using FixGuardian.Message.Attributes;
using FixGuardian.Message.Exceptions;
using System;
using System.Collections;
using System.Diagnostics;
using System.Reflection;
using System.Xml.Linq;
using static FixGuardian.Message.FixHelper;

namespace FixGuardian.Message
{
    public class Header : AMessage
    {
        [PositionalTag(1)]
        [Tag(8)]
        public string? BeginString { get; set; } = null;

        [PositionalTag(2)]
        [Tag(9)]
        public uint? BodyLength { get; set; } = null;

        [PositionalTag(3)]
        [Tag(35)]
        public char? MsgType { get; set; } = null;

        [Tag(49)]
        public string? SenderCompId { get; set; } = null;

        [Tag(56)]
        public string? TargetCompId { get; set; } = null;

        [Tag(34)]
        public uint? MsgSeqNum { get; set; } = null;

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
                pair.Key.SetValue(this, null);
        }

        public string ToString(FixHelper.NullHandlingStrategy context = FixHelper.NullHandlingStrategy.None)
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
                    switch (context)
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
                    strValue = FixHelper.GetEnumValue((Enum)value);
                else if (value is DateTime)
                    strValue += Convert.ToDateTime(value).ToString("yyyyMMdd-HH:mm:ss");
                else
                    strValue = value.ToString()!;
                result += $"{pair.Value.TagId}={strValue}\u0001";
            }
            return result;
        }

        public void FromString(List<KeyValuePair<ushort, string>> mapmsg)
        {
            HashSet<ushort> tagset = new HashSet<ushort>();
            Clear();

            foreach (KeyValuePair<PropertyInfo, Tag> prop in PositionalProperty)
            {
                if (mapmsg.Count == 0)
                    throw new FixDecodeException($"Missing positional {prop.Key.Name}"); // todo
                if (prop.Value.TagId != mapmsg[0].Key)
                    throw new FixDecodeException($"Incorrect position tag: '{prop.Value.TagId}' != '{mapmsg[0].Key}'"); // todo

                object? value = FixHelper.ConvertValue(mapmsg[0].Value, prop.Key.PropertyType);

                if (value == null)
                    throw new FixDecodeException($"Null value for property: {prop.Key.Name}");
                prop.Key.SetValue(this, value);
                mapmsg.RemoveAt(0);
            }
            for (int it = 0; 0 != mapmsg.Count; it++)
            {
                if (tagset.Contains(mapmsg[0].Key))
                    throw new FixDecodeException($"Key already parsed: {mapmsg[0].Key}");

                PropertyInfo? prop = GlobalProperty.Where(prop => prop.Value.TagId == mapmsg[0].Key).Select(prop => prop.Key).FirstOrDefault();

                if (prop == null)
                    break;

                object? value = FixHelper.ConvertValue(mapmsg[0].Value, prop.PropertyType);

                if (value == null)
                    throw new FixDecodeException($"Null value for property: {prop.Name}");
                prop.SetValue(this, value);
                tagset.Add(mapmsg[0].Key);
                mapmsg.RemoveAt(0);
            }

            foreach (PropertyInfo prop in TagProperty)
                if (prop.GetValue(this) == null && prop.GetCustomAttribute<OptionalTag>() == null)
                    throw new FixDecodeException($"Missing required tag: {prop.Name} ({prop.GetCustomAttribute<Tag>()!.TagId})");
        }
    }
}
