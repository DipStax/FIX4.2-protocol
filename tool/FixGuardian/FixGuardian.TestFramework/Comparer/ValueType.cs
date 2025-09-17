using FixGuardian.TestFramework.Comparer;

namespace FixGuardian.Message.Tests.Comparer
{
    static public class ValueTypeComparer
    {
        static public ComparerResult Equal(object? actual, object? expected)
        {
            if (!IsValidEntry(actual) || !IsValidEntry(expected))
                return ComparerResult.TypeNotSupported;
            return AreEqual(actual, expected) ? ComparerResult.ComparedEqual : ComparerResult.ComparedNotEqual;
        }

        static public bool IsValidEntry(object? obj)
        {
            return IsFPN(obj) || IsNumeric(obj);
        }

        static public bool IsFPN(object? obj)
        {
            return obj != null && (obj is double or float or decimal);
        }

        static public bool IsNumeric(object? obj)
        {
            return obj != null && (obj is ulong or long or uint or int or ushort or short);
        }

        static public bool AreEqual(object actual, object expected)
        {
            if (actual is double || expected is double)
                return AreEqual(Convert.ToDouble(actual), Convert.ToDouble(expected));
            if (actual is float || expected is float)
                return AreEqual(Convert.ToSingle(actual), Convert.ToSingle(expected));
            if (actual is decimal || expected is decimal)
                return AreEqual(Convert.ToDecimal(actual), Convert.ToDecimal(expected));

            if (actual is ulong || expected is ulong)
                return AreEqual(Convert.ToUInt64(actual), Convert.ToUInt64(expected));
            if (actual is long || expected is long)
                return AreEqual(Convert.ToInt64(actual), Convert.ToInt64(expected));

            if (actual is uint || expected is uint)
                return AreEqual(Convert.ToUInt32(actual), Convert.ToUInt32(expected));
            if (actual is int || expected is int)
                return AreEqual(Convert.ToInt32(actual), Convert.ToInt32(expected));

            if (actual is ushort || expected is ushort)
                return AreEqual(Convert.ToUInt16(actual), Convert.ToUInt16(expected));
            return AreEqual(Convert.ToInt16(actual), Convert.ToInt16(expected));
        }

        static public bool AreEqual<T>(T actual, T expected)
        {
            return actual.Equals(expected);
        }
    }
}
