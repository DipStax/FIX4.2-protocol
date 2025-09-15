using FixGuardian.Message.Attributes;
using System.Reflection;
using System.Runtime.Serialization;
using System.Security.Cryptography;

namespace FixGuardian.Message
{
    public class FixHelper
    {
        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T">Message type</typeparam>
        /// <param name="msg">List map of the message to decode</param>
        /// <param name="allowNull">Does the message allow null required tag</param>
        /// <returns></returns>
        /// <exception cref="Exception"></exception>
        static public string ToString<T>(T msg, bool allowNull = false)
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
                    if (!allowNull)
                        throw new Exception($"Null value for property: {pair.Key.Name}");
                    continue;
                }

                string strValue = string.Empty;
                Type underlying = Nullable.GetUnderlyingType(pair.Key.PropertyType) ?? pair.Key.PropertyType;

                if (underlying.IsEnum)
                    strValue = GetEnumValue((Enum)value);
                else if (value is DateTime)
                    strValue += Convert.ToDateTime(value).ToString("yyyyMMdd-HH:mm:ss");
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

        // static public T FromString<T>(List<KeyValuePair<ushort, string>> mapmsg)
        //     where T : new()
        // {
        //     T msgobj = new T();
        //     IEnumerable<KeyValuePair<PropertyInfo, Tag>> props = typeof(T)
        //         .GetProperties(BindingFlags.Public | BindingFlags.Instance)
        //         .Where(prop => prop.GetCustomAttribute<Tag>() != null)
        //         .Select(prop => new KeyValuePair<PropertyInfo, Tag>(prop, prop.GetCustomAttribute<Tag>()!));

        //     foreach (KeyValuePair<ushort, string> pair in mapmsg)
        //     {
        //         KeyValuePair<PropertyInfo, Tag>? prop = props.Where(prop => prop.Value.TagId == pair.Key).FirstOrDefault();

        //         if (prop == null)
        //             throw new Exception($"Undefined tag in message {typeof(T).Name}: {pair.Key}");
        //         prop.Key.SetValue(msgobj, )
        //     }

        //     return msgobj;
        // }

        static public string AddCheckSum(string message)
        {
            UInt32 checksum = 0;

            foreach (char c in message)
                checksum += c;
            message += $"10={checksum % 256}\u0001";
            return message;
        }

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
            if (string.IsNullOrEmpty(raw))
                return null;

            var underlying = Nullable.GetUnderlyingType(target) ?? target;

            if (underlying.IsEnum)
            {
                if (int.TryParse(raw, out int enumValue))
                    return Enum.ToObject(underlying, enumValue);
                return Enum.Parse(underlying, raw);
            }

            if (underlying == typeof(char))
                return raw[0];
            if (underlying == typeof(int))
                return int.Parse(raw);
            if (underlying == typeof(uint))
                return uint.Parse(raw);
            if (underlying == typeof(short))
                return short.Parse(raw);
            if (underlying == typeof(ushort))
                return ushort.Parse(raw);
            if (underlying == typeof(DateTime))
                return DateTime.ParseExact(raw, "yyyyMMdd-HH:mm:ss", null); // FIX standard timestamp
            if (underlying == typeof(string))
                return raw;

            // fallback: try ChangeType
            return Convert.ChangeType(raw, underlying);
        }
    }
}