using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FixGuardian.Message.Exceptions
{
    public class FixDecodeException : Exception
    {
        public FixDecodeException(string msg)
            : base(msg)
        {
        }
    }
}
