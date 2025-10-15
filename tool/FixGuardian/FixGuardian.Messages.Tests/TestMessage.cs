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

        public class OptionalMessageList
        {
            [Tag(7)]
            public uint? UintTag { get; set; } = null;

            [OptionalTag]
            [Tag(8)]
            public string? OptionalStringTag { get; set; } = null;
        }

        public class RequiredMessageList
        {
            public enum TestEnumList
            {
                Value1 = 1,
                [EnumMember(Value = "C")]
                Value2
            };

            [Tag(10)]
            public char? CharTag { get; set; } = null;

            [OptionalTag]
            [Tag(11)]
            public TestEnumList? OptionalEnumTag { get; set; } = null;
        }

        [Tag(0)]
        public string? StringTag { get; set; } = null;

        [Tag(1)]
        public ushort? UshortTag { get; set; } = null;

        [Tag(2)]
        public TestEnum? EnumTag { get; set; } = null;

        [OptionalTag]
        [Tag(3)]
        public string? OptionalStringTag { get; set; }

        [OptionalTag]
        [Tag(4)]
        public ushort? OptionalUShortTag { get; set; } = null;

        [OptionalTag]
        [Tag(5)]
        public TestEnum? OptionalEnumTag { get; set; } = null;

        [OptionalTag]
        [ListTag(6)]
        public List<OptionalMessageList>? OptionalListTag { get; set; } = null;

        [OptionalTag]
        [ListTag(9)]
        public List<RequiredMessageList>? ListTag { get; set; } = null;
    }
}
