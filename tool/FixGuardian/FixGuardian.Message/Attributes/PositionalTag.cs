using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FixGuardian.Message.Attributes
{
    public class PositionalTag : Attribute
    {
        public UInt32 TagId;
        public byte Position;

        public PositionalTag(byte position, UInt32 tagId)
        {
            TagId = tagId;
            Position = position;
        }
    }
}
