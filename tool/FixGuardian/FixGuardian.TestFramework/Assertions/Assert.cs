using FixGuardian.Messages;
using FixGuardian.Message.Tests.Comparer;
using FixGuardian.Messages.Definition;
using FixGuardian.Messages.Exceptions;
using FixGuardian.TestFramework.Comparer;

namespace FixGuardian.TestFramework.Assertions
{
    public class AssertionException : Exception
    {
        public object? Expected { get; }
        public object? Actual { get; }

        public bool WithValue { get; }

        public AssertionException(string msg)
            : base(msg)
        {
            WithValue = false;
        }

        public AssertionException(string msg, object? expected, object? actual)
            : base(msg)
        {
            Expected = expected;
            Actual = actual;
            WithValue = true;
        }

        public AssertionException(string msg, Exception inner)
            : base(msg, inner)
        {
            WithValue = false;
        }
    }

    public class Assert
    {
        private delegate ComparerResult ComparerMethod(object x, object y);

        static private readonly ComparerMethod[] Comparers =
        {
            ValueTypeComparer.Equal
        };

        static public (Header, T) Received<T>(string msg)
            where T : AMessage, new()
        {
            List<KeyValuePair<ushort, string>> msgmap;
            Header header = new Header();
            T message;

            try
            {
                msgmap = Mapable(msg);
                ValidCheckSum(msgmap);
                msgmap.RemoveAt(msgmap.Count - 1);
                header.FromString(msgmap);
                message = FixHelper.FromString<T>(msgmap);
            }
            catch (AssertionException ex)
            {
                throw new AssertionException("Mapping failed", ex);
            }
            catch (FixDecodeException ex)
            {
                throw new AssertionException("Decoding message failed", ex);
            }
            return (header, message);
        }

        static public List<KeyValuePair<ushort, string>> Mapable(string msg)
        {
            try
            {
                return FixHelper.ToMap(msg);
            }
            catch (FixDecodeException ex)
            {
                throw new AssertionException("Mapping failed", ex);
            }
        }

        static public void ValidCheckSum(List<KeyValuePair<ushort, string>> mapmsg)
        {
            uint checksum = 0;

            try
            {
                Equal(mapmsg.Last().Key, 10);
            }
            catch (AssertionException ex)
            {
                throw new AssertionException("Checksum tag invalid", ex);
            }

            for (int i = 0; i < mapmsg.Count - 1; i++)
            {
                foreach (char c in mapmsg[i].Key.ToString() + mapmsg[i].Value)
                    checksum += c;
                checksum += '=' + 1;
            }
            try
            {
                Equal(uint.Parse(mapmsg.Last().Value), checksum % 256);
            }
            catch (AssertionException ex)
            {
                throw new AssertionException("CheckSum value invalid", ex);
            }
        }

        static public void NotNull(object? obj)
        {
            if (obj == null)
                throw new AssertionException("Value is null", null, obj);
        }

        static public void Equal(object? actual, object? expected)
        {
            if (actual is null && expected is null)
                return;
            if (actual is null || expected is null)
                throw new AssertionException("One of the comparent is null", expected, actual);

            if (ReferenceEquals(actual, expected))
                return;

            foreach (ComparerMethod comparer in Comparers)
            {
                switch (comparer(actual, expected))
                {
                    case ComparerResult.TypeNotSupported:
                        continue;
                    case ComparerResult.ComparedNotEqual:
                        throw new AssertionException($"Not Equal", expected, actual);
                    case ComparerResult.ComparedEqual:
                        return;
                }
            }
            if (!actual.Equals(expected))
                throw new AssertionException($"Not Equal", expected, actual);
        }
    }
}