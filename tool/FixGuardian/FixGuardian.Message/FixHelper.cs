using FixGuardian.Message.Attributes;
using FixGuardian.Message.Exceptions;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.Globalization;
using System.Reflection;
using System.Runtime.Serialization;
using System.Security.Cryptography;
using static FixGuardian.Message.Header;

namespace FixGuardian.Message
{
    public class FixHelper
    {
        public enum NullHandlingStrategy
        {
            None,
            AllowNull,
            NullAsEmpty,
            NullAsFullyEmpty
        };

        static public string ToString<T>(T msg, NullHandlingStrategy nullStrat = NullHandlingStrategy.None)
        {
            IEnumerable<KeyValuePair<PropertyInfo, Tag>> props = typeof(T)
                .GetProperties(BindingFlags.Public | BindingFlags.Instance)
                .Where(prop => prop.GetCustomAttribute<Tag>() != null)
                .Select(prop => new KeyValuePair<PropertyInfo, Tag>(prop, prop.GetCustomAttribute<Tag>()!));
            string result = string.Empty;

            foreach (KeyValuePair<PropertyInfo, Tag> pair in props)
            {
                object? value = pair.Key.GetValue(msg);

                if (value == null && pair.Key.GetCustomAttribute<OptionalTag>() != null)
                {
                    continue;
                }
                else if (value == null)
                {
                    switch (nullStrat)
                    {
                        case NullHandlingStrategy.None:
                            throw new FixEncodeException($"Null value for property", pair.Value.TagId);
                        case NullHandlingStrategy.NullAsEmpty:
                            result += $"{pair.Value.TagId}=\u0001";
                            continue;
                        case NullHandlingStrategy.NullAsFullyEmpty:
                            result += "\u0001";
                            continue;
                        case NullHandlingStrategy.AllowNull:
                            continue;
                        default:
                            throw new UnreachableException();
                    }
                }
                string strValue = string.Empty;
                Type underlying = Nullable.GetUnderlyingType(pair.Key.PropertyType) ?? pair.Key.PropertyType;


                if (underlying.IsEnum)
                    strValue = GetEnumValue((Enum)value);
                else if (value is DateTime)
                    strValue = Convert.ToDateTime(value).ToString("yyyyMMdd-HH:mm:ss");
                else
                    strValue = value.ToString()!;
                result += $"{pair.Value.TagId}={strValue}\u0001";
            }
            return result;
        }

        static public List<KeyValuePair<ushort, string>> ToMap(string msg)
        {
            List<string> fields = msg.Split('\u0001').ToList();
            List<KeyValuePair<ushort, string>> result = new List<KeyValuePair<ushort, string>>();

            if (fields.Last() != string.Empty)
                throw new Exception("Message trailing is not empty");
            fields.RemoveAt(fields.Count - 1);
            if (fields.Where(string.IsNullOrWhiteSpace).Count() != 0)
                throw new Exception("Message split with empty value");
            foreach (string field in fields)
            {
                string[] kv = field.Split('=');

                if (kv.Length != 2)
                    throw new Exception("Message contain multiple '=' in a single field");
                result.Add(new KeyValuePair<ushort, string>(Convert.ToUInt16(kv[0]), kv[1]));
            }
            return result;
        }

        static public T FromString<T>(List<KeyValuePair<ushort, string>> mapmsg)
             where T : new()
        {
            T msgobj = new T();
            IEnumerable<KeyValuePair<PropertyInfo, Tag>> props = typeof(T)
                .GetProperties(BindingFlags.Public | BindingFlags.Instance)
                .Where(prop => prop.GetCustomAttribute<Tag>() != null)
                .Select(prop => new KeyValuePair<PropertyInfo, Tag>(prop, prop.GetCustomAttribute<Tag>()!));

            foreach (KeyValuePair<ushort, string> pair in mapmsg)
            {
                KeyValuePair<PropertyInfo, Tag>? prop = props
                    .Where(prop => prop.Value.TagId == pair.Key)
                    .Cast<KeyValuePair<PropertyInfo, Tag>?>()
                    .FirstOrDefault();

                if (prop == null)
                    throw new FixDecodeException($"Undefined tag in message {typeof(T).Name}: '{pair.Key}'");

                object? value = ConvertValue(pair.Value, Nullable.GetUnderlyingType(prop.Value.Key.PropertyType) ?? prop.Value.Key.PropertyType);

                if (value == null)
                    throw new FixDecodeException("");
                prop.Value.Key.SetValue(msgobj, value);
            }

            foreach (PropertyInfo prop in props.Select(prop => prop.Key))
                if (prop.GetValue(msgobj) == null && prop.GetCustomAttribute<OptionalTag>() == null)
                    throw new FixDecodeException($"Missing required tag: {prop.Name} ({prop.GetCustomAttribute<Tag>()!.TagId})");

            return msgobj;
        }

        static public string AddCheckSum(string message)
        {
            int checksum = 0;

            foreach (char c in message)
                checksum += c;
            return AddArbitraryCheckSum(message, checksum % 256);
        }

        static public string AddArbitraryCheckSum(string message, int checksum)
        {
             return $"{message}10={checksum}\u0001";
        }

        #region Value Convertion

        private static readonly ConcurrentDictionary<Type, Dictionary<string, object>> EnumMemberCache = new ConcurrentDictionary<Type, Dictionary<string, object>>();
        private static readonly Dictionary<Type, Func<string, object>> Parsers = new Dictionary<Type, Func<string, object>>()
        {
            { typeof(char), raw => raw[0] },
            { typeof(int), raw => int.Parse(raw, CultureInfo.InvariantCulture) },
            { typeof(uint), raw => uint.Parse(raw, CultureInfo.InvariantCulture) },
            { typeof(short), raw => short.Parse(raw, CultureInfo.InvariantCulture) },
            { typeof(ushort), raw => ushort.Parse(raw, CultureInfo.InvariantCulture) },
            { typeof(DateTime), raw => DateTime.ParseExact(raw, "yyyyMMdd-HH:mm:ss", CultureInfo.InvariantCulture) },
            { typeof(string), raw => raw }
        };

        static public string GetEnumValue(Enum value)
        {
            MemberInfo? member = value.GetType().GetMember(value.ToString()).FirstOrDefault();

            if (member != null)
            {
                var attr = member.GetCustomAttribute<EnumMemberAttribute>();

                if (attr != null && !string.IsNullOrEmpty(attr.Value))
                    return attr.Value;
            }
            return Convert.ToInt32(value).ToString();
        }

        static public object? ConvertValue(string raw, Type target)
        {

            var underlying = Nullable.GetUnderlyingType(target) ?? target;

            if (string.IsNullOrEmpty(raw) && underlying != typeof(string))
                return null;
            if (underlying.IsEnum)
            {
                if (int.TryParse(raw, out int enumValue))
                    return Enum.ToObject(underlying, enumValue);

                var map = EnumMemberCache.GetOrAdd(underlying, t =>
                {
                    return t.GetFields(BindingFlags.Public | BindingFlags.Static)
                            .Select(f =>
                            {
                                var attr = f.GetCustomAttribute<EnumMemberAttribute>();
                                var name = attr?.Value ?? f.Name;
                                return new KeyValuePair<string, object>(name, f.GetValue(null)!);
                            })
                            .ToDictionary(kvp => kvp.Key, kvp => kvp.Value,
                                          StringComparer.OrdinalIgnoreCase);
                });

                if (map.TryGetValue(raw, out var enumConst))
                    return enumConst;
                return Enum.Parse(underlying, raw, ignoreCase: true);
            }

            if (Parsers.TryGetValue(underlying, out var parser))
                return parser(raw);
            return Convert.ChangeType(raw, underlying);
        }

        #endregion
    }
}