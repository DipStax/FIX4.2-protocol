using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.Exceptions;
using FixGuardian.Messages.Definition;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.Globalization;
using System.Reflection;
using System.Runtime.Serialization;
using System.Collections;

namespace FixGuardian.Messages
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

        static public string ToString(object msgobj, NullHandlingStrategy nullStrat = NullHandlingStrategy.None)
        {
            IEnumerable<PropertyInfo> props = msgobj.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance);
            IEnumerable<PropertyInfo> tagProps = props.Where(prop => prop.GetCustomAttribute<Tag>() != null);
            IEnumerable<PropertyInfo> listProps = props.Where(prop => prop.GetCustomAttribute<ListTag>() != null);
            string result = string.Empty;

            foreach (PropertyInfo prop in listProps)
            {
                object? propObj = prop.GetValue(msgobj);
                ushort noTag = prop.GetCustomAttribute<ListTag>()!.NoTag;

                if (propObj == null && prop.GetCustomAttribute<OptionalTag>() != null)
                {
                    continue;
                }
                else if (propObj == null)
                {
                    switch (nullStrat)
                    {
                        case NullHandlingStrategy.None:
                            throw new FixEncodeException($"Null value for property", noTag);
                        case NullHandlingStrategy.NullAsEmpty:
                            result += $"{noTag}=\u0001";
                            break;
                        case NullHandlingStrategy.NullAsFullyEmpty:
                            result += "\u0001";
                            break;
                        case NullHandlingStrategy.AllowNull:
                            break;
                        default:
                            throw new UnreachableException();
                    }
                }
                else
                {
                    if (propObj is IEnumerable listObj)
                    {
                        result += $"{noTag}={listObj.Cast<object>().Count()}\u0001";
                        foreach (object obj in listObj)
                            result += ToString(obj, nullStrat);
                    }
                    else
                    {
                        throw new FixEncodeException("", noTag);
                    }
                }
            }
            foreach (PropertyInfo prop in tagProps)
            {
                object? value = prop.GetValue(msgobj);
                ushort tag = prop.GetCustomAttribute<Tag>()!.TagId;

                if (value == null && prop.GetCustomAttribute<OptionalTag>() != null)
                {
                    continue;
                }
                else if (value == null)
                {
                    switch (nullStrat)
                    {
                        case NullHandlingStrategy.None:
                            throw new FixEncodeException($"Null value for non-optional property", tag);
                        case NullHandlingStrategy.NullAsEmpty:
                            result += $"{tag}=\u0001";
                            break;
                        case NullHandlingStrategy.NullAsFullyEmpty:
                            result += "\u0001";
                            break;
                        case NullHandlingStrategy.AllowNull:
                            break;
                        default:
                            throw new UnreachableException();
                    }
                }
                else
                {
                    string strValue = string.Empty;
                    Type underlying = Nullable.GetUnderlyingType(prop.PropertyType) ?? prop.PropertyType;

                    if (underlying.IsEnum)
                        strValue = GetEnumValue((Enum)value);
                    else if (value is DateTime)
                        strValue = Convert.ToDateTime(value).ToString("yyyyMMdd-HH:mm:ss");
                    else
                        strValue = value.ToString()!;
                    result += $"{tag}={strValue}\u0001";
                }
            }
            return result;
        }

        static public List<KeyValuePair<ushort, string>> ToMap(string msg)
        {
            List<string> fields = msg.Split('\u0001').ToList();
            List<KeyValuePair<ushort, string>> result = new List<KeyValuePair<ushort, string>>();

            if (fields.Last() != string.Empty)
                throw new FixDecodeException("Message trailing is not empty");
            fields.RemoveAt(fields.Count - 1);
            if (fields.Where(string.IsNullOrWhiteSpace).Count() != 0)
                throw new FixDecodeException("Message split with empty value");
            foreach (string field in fields)
            {
                string[] kv = field.Split('=');

                if (kv.Length != 2)
                    throw new FixDecodeException("Message contain multiple '=' in a single field");
                result.Add(new KeyValuePair<ushort, string>(Convert.ToUInt16(kv[0]), kv[1]));
            }
            return result;
        }

        public enum OutReason
        {
            AlreadyParsed,
            NotFound,
            Success
        }

        static public T FromString<T>(List<KeyValuePair<ushort, string>> mapmsg)
            where T : AMessage, new()
        {
            T msgobj = new T();
            int index = 0;

            if (FromString(mapmsg, msgobj, ref index) != OutReason.Success)
                throw new FixDecodeException("w");
            return msgobj;
        }

        static private OutReason FromString(List<KeyValuePair<ushort, string>> mapmsg, object msgobj, ref int index)
        {
            Type msgtype = msgobj.GetType();
            IEnumerable<PropertyInfo> props = msgtype.GetProperties(BindingFlags.Public | BindingFlags.Instance);
            IEnumerable<PropertyInfo> tagProps = props.Where(prop => prop.GetCustomAttribute<Tag>() != null);
            IEnumerable<PropertyInfo> listProps = props.Where(prop => prop.GetCustomAttribute<ListTag>() != null);
            SortedSet<ushort> setTag = new SortedSet<ushort>();
            OutReason outReason = OutReason.Success;

            while (index < mapmsg.Count)
            {
                KeyValuePair<ushort, string> pair = mapmsg[index];
                PropertyInfo? tag = tagProps.FirstOrDefault(prop => prop.GetCustomAttribute<Tag>()!.TagId == pair.Key);

                if (setTag.Contains(pair.Key))
                {
                    outReason = OutReason.AlreadyParsed;
                    break;
                }
                setTag.Add(pair.Key);
                if (tag != null)
                {
                    Type underlyingType = Nullable.GetUnderlyingType(tag.PropertyType) ?? tag.PropertyType;
                    object? value = ConvertValue(pair.Value, underlyingType);

                    if (value == null)
                        throw new FixDecodeException($"Unable to convert the value of the tag: {tag.Name} ({pair.Key}) -> '{pair.Value}' to {underlyingType.Name}");
                    tag.SetValue(msgobj, value);
                    index++;
                }
                else
                {
                    PropertyInfo? list = listProps.FirstOrDefault(prop => prop.GetCustomAttribute<ListTag>()!.NoTag == pair.Key);

                    if (list != null)
                    {
                        if (!uint.TryParse(pair.Value, out uint groupCount))
                            throw new FixDecodeException($"Invalid repeating group count for tag {pair.Key}: '{pair.Value}'");

                        Type elementType = list.PropertyType.GetGenericArguments().FirstOrDefault() ?? list.PropertyType.GetElementType() ?? throw new FixDecodeException($"Cannot determine element type of ListTag '{list .Name}'");
                        object listInstance = Activator.CreateInstance(typeof(List<>).MakeGenericType(elementType))!;
                        MethodInfo addMethod = listInstance.GetType().GetMethod("Add")!;

                        index++;
                        for (uint group = 0; group < groupCount; group++)
                        {
                            try
                            {
                                object groupObj = Activator.CreateInstance(elementType)!;
                                OutReason reason = FromString(mapmsg, groupObj, ref index);

                                if (reason == OutReason.NotFound && group != groupCount - 1)
                                    throw new FixDecodeException($"Unable to parse full group, tag not found: {mapmsg[index].Key} -> '{mapmsg[index].Value}'");
                                addMethod.Invoke(listInstance, [groupObj]);
                            }
                            catch (FixDecodeException ex)
                            {
                                throw new FixDecodeException($"Exception when parsing group: '{elementType.Name}' at index: {group}", ex);
                            }
                        }
                    }
                    else
                    {
                        outReason = OutReason.NotFound;
                        break;
                    }
                }
            }
            foreach (PropertyInfo prop in tagProps)
                if (prop.GetValue(msgobj) == null && prop.GetCustomAttribute<OptionalTag>() == null)
                    throw new FixDecodeException($"Missing required tag in '{msgtype.Name}': {prop.Name} ({prop.GetCustomAttribute<Tag>()!.TagId})");
            foreach (PropertyInfo prop in listProps)
                if (prop.GetValue(msgobj) == null && prop.GetCustomAttribute<OptionalTag>() == null)
                    throw new FixDecodeException($"Missing required list in '{msgtype.Name}': {prop.Name} ({prop.GetCustomAttribute<ListTag>()!.NoTag})");
            return outReason;
        }


        #region CheckSum

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

        #endregion

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