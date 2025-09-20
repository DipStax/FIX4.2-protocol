using FixGuardian.TestFramework;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.Messages.Definition;
using FixGuardian.TestFramework.Attributes;
using System.Threading.Tasks;

namespace FixGuardian.Suite
{
    [RequireSuite("Logon validation")]
    [TestSuite("Logout validation")]
    public class LogoutValidation
    {
        [TestCase("Valide Logout")]
        public void ValidLogout()
        {
            Client client = new Client("Sender");

            client.Logon();

            client.Send(new Logout());
            Logout _ = client.Receive<Logout>();

            client.AssertServerDisconnet();
        }
    }
}