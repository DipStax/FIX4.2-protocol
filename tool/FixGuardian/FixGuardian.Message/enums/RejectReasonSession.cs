namespace FixGuardian.Messages.Enums
{
    public enum RejectReasonSession
    {
        InvalidTagNum = 0,
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

