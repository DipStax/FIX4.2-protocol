using FixGuardian.Message.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace FixGuardian.Message
{
    public class AMessage : IEquatable<AMessage>
    {
        public override string ToString()
        {
            string result = string.Empty;
            IEnumerable<KeyValuePair<PropertyInfo, Tag>> props = GetType()
                .GetProperties(BindingFlags.Public | BindingFlags.Instance)
                .Where(prop => prop.GetCustomAttribute<Tag>() != null)
                .Select(prop => new KeyValuePair<PropertyInfo, Tag>(prop, prop.GetCustomAttribute<Tag>()!));

            foreach (KeyValuePair<PropertyInfo, Tag> prop in props)
                result += $"{prop.Key.Name} ({prop.Value.TagId}) = '{prop.Key.GetValue(this)}'\n";
            return result;
        }

        public bool Equals(AMessage? other)
        {
            IEnumerable<KeyValuePair<PropertyInfo, Tag>> props = GetType()
                .GetProperties(BindingFlags.Public | BindingFlags.Instance)
                .Where(prop => prop.GetCustomAttribute<Tag>() != null)
                .Select(prop => new KeyValuePair<PropertyInfo, Tag>(prop, prop.GetCustomAttribute<Tag>()!));

            foreach (KeyValuePair<PropertyInfo, Tag> prop in props)
            {
                object? lhs = prop.Key.GetValue(this);
                object? rhs = prop.Key.GetValue(other);

                if (lhs is null && rhs is null)
                    continue;
                if (lhs is null || rhs is null)
                    return false;

                if (lhs is DateTime dt1 && rhs is DateTime dt2)
                {
                    dt1 = dt1.AddTicks(-(dt1.Ticks % TimeSpan.TicksPerSecond));
                    dt2 = dt2.AddTicks(-(dt2.Ticks % TimeSpan.TicksPerSecond));

                    if (dt1 != dt2)
                        return false;
                }
                else if (!Equals(lhs, rhs))
                {
                    return false;
                }
            }
            return true;
        }
    }
}
