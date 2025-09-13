using FixGuardian.Message.Attributes;

using System.Reflection;

namespace FixGuardian.Message
{
    public class Header
    {
        [PositionalTag(1, 8)]
        public string? BeginString { get; set; } = null;

        [PositionalTag(2, 9)]
        public UInt32? BodyLength { get; set; } = null;

        [PositionalTag(3, 35)]
        public char? MsgType { get; set; } = null;

        [Tag(49)]
        public string? SenderCompId { get; set; } = null;

        [Tag(56)]
        public string? TargetCompId { get; set; } = null;

        [Tag(34)]
        public UInt32? MsgSeqNum { get; set; } = null;

        [Tag(52)]
        public DateTime? SendingTime { get; set; } = null;

        public static string ToString(Header header)
        {
            PropertyInfo[] properties = typeof(Header).GetProperties(BindingFlags.Public | BindingFlags.Instance);
            List<KeyValuePair<PropertyInfo, PositionalTag>> positional = new List<KeyValuePair<PropertyInfo, PositionalTag>>();
            List<KeyValuePair<PropertyInfo, Tag>> tag = new List<KeyValuePair<PropertyInfo, Tag>>();
            string result = string.Empty;

            foreach (PropertyInfo prop in properties)
            {
                PositionalTag? posAttribute = prop.GetCustomAttribute<PositionalTag>();
                Tag? tagAttribute = prop.GetCustomAttribute<Tag>();

                if (posAttribute != null)
                    positional.Add(new KeyValuePair<PropertyInfo, PositionalTag>(prop, posAttribute));
                else if (tagAttribute != null)
                    tag.Add(new KeyValuePair<PropertyInfo, Tag>(prop, tagAttribute));
            }
            foreach (KeyValuePair<PropertyInfo, PositionalTag> pair in positional.OrderBy(pair => pair.Value.Position))
            {
                object? value = pair.Key.GetValue(header);

                if (value == null)
                    continue;
                    //throw new Exception($"Null value for property: {pair.Key.Name}");
                if (value is DateTime)
                    result += $"{pair.Value.TagId}={Convert.ToDateTime(value).ToString("yyyyMMdd-HH:mm:ss")}\u0001";
                else
                    result += $"{pair.Value.TagId}={value.ToString()}\u0001";
            }
            foreach (KeyValuePair<PropertyInfo, Tag> pair in tag)
            {
                object? value = pair.Key.GetValue(header);

                if (value == null)
                    continue;
                    //throw new Exception($"Null value for property: {pair.Key.Name}");
                if (value is DateTime)
                    result += $"{pair.Value.TagId}={Convert.ToDateTime(value).ToString("yyyyMMdd-HH:mm:ss")}\u0001";
                else
                    result += $"{pair.Value.TagId}={value.ToString()}\u0001";
            }
            return result;
        }
    }
}
