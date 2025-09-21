using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.Definition;
using System.Runtime.Serialization;

namespace FixGuardian.Messages.Tests
{
    public class TestMessage : AMessage
    {
        public enum TestEnum
        {
            Value1 = 0,
            [EnumMember(Value = "A")]
            Value2
        };

        [Tag(0)]
        public string? StringTag { get; set; } = null;

        [Tag(1)]
        public ushort? UshortTag { get; set; } = null;

        [Tag(2)]
        public TestEnum? EnumTag { get; set; } = null;

        [OptionalTag()]
        [Tag(3)]
        public string? OptionalStringTag { get; set; }

        [OptionalTag()]
        [Tag(4)]
        public ushort? OptionalUShortTag { get; set; } = null;

        [OptionalTag()]
        [Tag(5)]
        public TestEnum? OptionalEnumTag { get; set; } = null;
    }
}
