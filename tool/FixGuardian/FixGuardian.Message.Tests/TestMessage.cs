using FixGuardian.Message.Attributes;
using Microsoft.VisualStudio.TestPlatform.CommunicationUtilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace FixGuardian.Message.Tests
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
