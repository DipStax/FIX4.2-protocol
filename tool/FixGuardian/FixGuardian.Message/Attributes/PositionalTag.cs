using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FixGuardian.Message.Attributes
{
    public class PositionalTag : Attribute
    {
        public byte Position;

        public PositionalTag(byte position)
        {
            Position = position;
        }
    }
}
