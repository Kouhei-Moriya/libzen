package test.zen.generated;
import org.junit.Test;
import zen.main.ZenMain;
public class BinaryOperatorTest {
    @Test
    public void test() {
        ZenMain.ExecCommand(new String[] {"-l", "jvm", "test/zen/BinaryOperator.zen"});
    }
}
