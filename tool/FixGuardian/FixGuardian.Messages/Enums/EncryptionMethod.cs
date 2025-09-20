namespace FixGuardian.Messages.Enums
{
    public enum EncryptionMethod
    {
        None = 0,
        PKCS,
        DES,
        PKCSDES,
        PGPDES,
        PGPDESMD5,
        PEMDESMD5
    };
}