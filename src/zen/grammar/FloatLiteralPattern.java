package zen.grammar;

import zen.ast.ZFloatNode;
import zen.ast.ZNode;
import zen.deps.LibZen;
import zen.deps.Var;
import zen.deps.ZMatchFunction;
import zen.parser.ZToken;
import zen.parser.ZTokenContext;

public class FloatLiteralPattern extends ZMatchFunction {

	@Override public ZNode Invoke(ZNode ParentNode, ZTokenContext TokenContext, ZNode LeftNode) {
		@Var ZToken Token = TokenContext.GetToken(ZTokenContext._MoveNext);
		return new ZFloatNode(ParentNode, Token, LibZen._ParseFloat(Token.GetText()));
	}

}
