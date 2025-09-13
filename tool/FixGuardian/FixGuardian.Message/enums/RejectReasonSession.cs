using System;

namespace FixGuardian.Message.enums
{
    public enum RejectReasonSession
    {
        InvalidTagNum,
        RequiredTagMissing,
        TagNotDefine,
        UndefineTag,
        TagWithoutValue,
        ValueOutOfRange,
        IncorrectDataFormat,
        SignatureProblem,
        CompIDProblem,
        SendingTimeAccuracy,
        InvalidMsgType
    }
}

