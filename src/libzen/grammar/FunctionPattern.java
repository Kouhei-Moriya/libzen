package libzen.grammar;

import zen.ast.ZFunctionNode;
import zen.ast.ZNode;
import zen.deps.Var;
import zen.deps.ZenMatchFunction;
import zen.parser.ZTokenContext;

public class FunctionPattern extends ZenMatchFunction {

	@Override public ZNode Invoke(ZNode ParentNode, ZTokenContext TokenContext, ZNode LeftNode) {
		@Var ZNode FuncNode = new ZFunctionNode(ParentNode);
		FuncNode = TokenContext.MatchToken(FuncNode, "function", ZTokenContext.Required);
		FuncNode = TokenContext.MatchPattern(FuncNode, ZNode.NameInfo, "$Name$", ZTokenContext.Optional);
		FuncNode = TokenContext.MatchNtimes(FuncNode, "(", "$Param$", ",", ")");
		FuncNode = TokenContext.MatchPattern(FuncNode, ZNode.TypeInfo, "$TypeAnnotation$", ZTokenContext.Optional);
		FuncNode = TokenContext.MatchPattern(FuncNode, ZFunctionNode.Block, "$Block$", ZTokenContext.Required);
		return FuncNode;
	}

}