package libzen.grammar;

import zen.ast.ZBinaryNode;
import zen.ast.ZNode;
import zen.deps.Var;
import zen.deps.ZenMatchFunction;
import zen.parser.ZToken;
import zen.parser.ZTokenContext;

public class BinaryPattern extends ZenMatchFunction {

	@Override public ZNode Invoke(ZNode ParentNode, ZTokenContext TokenContext, ZNode LeftNode) {
		@Var ZToken Token = TokenContext.GetToken(ZTokenContext.MoveNext);
		@Var ZBinaryNode BinaryNode = new ZBinaryNode(ParentNode, Token, LeftNode, TokenContext.GetApplyingSyntax());
		return BinaryNode.AppendParsedRightNode(ParentNode, TokenContext);
	}

}