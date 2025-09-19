using FixGuardian.TestFramework;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.Messages.Definition;
using FixGuardian.TestFramework.Attributes;
using System.Threading.Tasks;

namespace FixGuardian.Suite
{
    [TestSuite("Logout Validation")]
    public class LogoutValidation
    {
        [TestCase("Valide Logout")]
        public void ValidLogout()
        {
            Client client = new Client("Sender");

            client.Logon();

            Logout _ = client.Receive<Logout>();
            client.AssertServerDisconnet();
        }
    }
}