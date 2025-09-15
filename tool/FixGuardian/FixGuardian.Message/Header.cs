using FixGuardian.Message.Attributes;
using System.Collections;
using System.Diagnostics;
using System.Reflection;

namespace FixGuardian.Message
{
    public class Header
    {
        [PositionalTag(1)]
        [Tag(8)]
        public string? BeginString { get; set; } = null;

        [PositionalTag(2)]
        [Tag(9)]
        public UInt32? BodyLength { get; set; } = null;

        [PositionalTag(3)]
        [Tag(35)]
        public char? MsgType { get; set; } = null;

        [Tag(49)]
        public string? SenderCompId { get; set; } = null;

        [Tag(56)]
        public string? TargetCompId { get; set; } = null;

        [Tag(34)]
        public UInt32? MsgSeqNum { get; set; } = null;

        [Tag(52)]
        public DateTime? SendingTime { get; set; } = null;

        static private IEnumerable<PropertyInfo> TagProperty = typeof(Header)
            .GetProperties(BindingFlags.Public | BindingFlags.Instance)
            .Where(prop => prop.GetCustomAttribute<Tag>() != null);

        static private IEnumerable<KeyValuePair<PropertyInfo, Tag>> GlobalProperty = TagProperty
            .Select(prop => new KeyValuePair<PropertyInfo, Tag>(prop, prop.GetCustomAttribute<Tag>()!))
            .OrderBy(pair =>
            {
                PositionalTag? attribute = pair.Key.GetCustomAttribute<PositionalTag>();

                if (attribute == null)
                    return byte.MaxValue;
                return attribute.Position;
            });

        static private IEnumerable<KeyValuePair<PropertyInfo, Tag>> PositionalProperty = GlobalProperty
            .Where(prop => prop.Key.GetCustomAttribute<PositionalTag>() != null);

        public void Clear()
        {
            foreach (KeyValuePair<PropertyInfo, Tag> pair in GlobalProperty)
            {
                pair.Key.SetValue(this, null);
            }
        }

        public string ToString(bool allowNull = false)
        {
            string result = string.Empty;

            foreach (KeyValuePair<PropertyInfo, Tag> pair in GlobalProperty)
            {
                object? value = pair.Key.GetValue(this);

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
                    strValue = FixHelper.GetEnumValue((Enum)value);
                else if (value is DateTime)
                    strValue += Convert.ToDateTime(value).ToString("yyyyMMdd-HH:mm:ss");
                else
                    strValue = value.ToString()!;
                result += $"{pair.Value.TagId}={strValue}\u0001";
            }
            return result;
        }

        public void FromString(List<KeyValuePair<ushort, string>> mapmsg, bool allowNull = false)
        {
            HashSet<ushort> tagset = new HashSet<ushort>();
            Clear();

            foreach (KeyValuePair<PropertyInfo, Tag> prop in PositionalProperty)
            {
                Console.WriteLine($"{prop.Key.Name} => {prop.Value.TagId} == {mapmsg[0].Key}|{mapmsg[0].Value}");
                if (mapmsg.Count == 0)
                    throw new Exception($"Missing positional {prop.Key.Name}"); // todo
                if (prop.Value.TagId != mapmsg[0].Key)
                    throw new Exception($"Incorrect position tag: '{prop.Value.TagId}' != '{mapmsg[0].Key}'"); // todo

                object? value = FixHelper.ConvertValue(mapmsg[0].Value, prop.Key.PropertyType);

                if (value == null && allowNull)
                {
                    Console.WriteLine($"Skipping {value == null && prop.Key.GetCustomAttribute<OptionalTag>() != null} || {allowNull}");
                    continue;
                }
                else if (value == null)
                    throw new Exception($"Null value for property: {prop.Key.Name}");
                prop.Key.SetValue(this, value);
                Console.WriteLine($"Set positional: {prop.Key.Name} as '{value}'");
                mapmsg.RemoveAt(0);
            }
            for (int it = 0; 0 != mapmsg.Count; it++)
            {
                if (tagset.Contains(mapmsg[0].Key))
                    throw new Exception($"Key already parsed: {mapmsg[0].Key}");

                PropertyInfo? prop = GlobalProperty.Where(prop => prop.Value.TagId == mapmsg[0].Key).Select(prop => prop.Key).FirstOrDefault();

                if (prop == null)
                    break;

                if (mapmsg[0].Value == string.Empty)
                {
                    if (allowNull || prop.GetCustomAttribute<OptionalTag>() != null)
                    {
                        Console.WriteLine($"{prop.Name} stay null");
                        tagset.Add(mapmsg[0].Key);
                        mapmsg.RemoveAt(0);
                        continue;
                    }
                    else
                    {
                        throw new Exception($"Null value for property: {prop.Name}");
                    }
                }
                else
                {
                    object? value = FixHelper.ConvertValue(mapmsg[0].Value, prop.PropertyType);

                    if (value == null)
                        throw new Exception($"Null value for property: {prop.Name}");
                    Console.WriteLine($"Setting tag {prop.Name} to: {value}");
                    prop.SetValue(this, value);
                    tagset.Add(mapmsg[0].Key);
                    mapmsg.RemoveAt(0);
                }
            }

            if (!allowNull)
                foreach (PropertyInfo prop in TagProperty)
                    if (prop.GetValue(this) == null && prop.GetCustomAttribute<OptionalTag>() == null)
                        throw new Exception($"Missing required tag: {prop.Name} ({prop.GetCustomAttribute<Tag>()!.TagId})");
        }
    }
}
