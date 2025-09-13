using FixGuardian.Message.Attributes;
using System.Reflection;

namespace FixGuardian.Message
{
    public class MessageSupport
    {
        public static string ToString<T>(T message, bool alowNull = false)
        {
            IEnumerable<KeyValuePair<PropertyInfo, Tag>> props = typeof(T)
                .GetProperties(BindingFlags.Public | BindingFlags.Instance)
                .Where(prop => prop.GetCustomAttribute<Tag>() != null)
                .Select(prop => new KeyValuePair<PropertyInfo, Tag>(prop, prop.GetCustomAttribute<Tag>()!))
                .OrderBy(pair => {
                    PositionalTag? attribute = pair.Key.GetCustomAttribute<PositionalTag>();

                    if (attribute == null)
                        return byte.MaxValue;
                    return attribute.Position;
                });;
            string result = string.Empty;

            foreach (KeyValuePair<PropertyInfo, Tag> pair in props)
            {
                object? value = pair.Key.GetValue(message);

                if (value == null)
                {
                    if (!alowNull)
                        throw new Exception($"Null value for property: {pair.Key.Name}");
                    continue;
                }
                if (value is DateTime)
                    result += $"{pair.Value.TagId}={Convert.ToDateTime(value).ToString("yyyyMMdd-HH:mm:ss")}\u0001";
                else
                    result += $"{pair.Value.TagId}={value.ToString()}\u0001";
            }
            return result;
        }
    }
}