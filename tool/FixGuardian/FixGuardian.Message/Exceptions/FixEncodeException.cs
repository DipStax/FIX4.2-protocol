using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FixGuardian.Message.Exceptions
{
    public class FixEncodeException : Exception
    {
        public uint RefTagId { get; private set; }

        public FixEncodeException(string msg, uint tagid)
            : base(msg)
        {
            RefTagId = tagid;
        }
    }
}
