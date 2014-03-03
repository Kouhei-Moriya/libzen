// ***************************************************************************
// Copyright (c) 2013, JST/CREST DEOS project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// *  Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// **************************************************************************

//ifdef JAVA
package zen.codegen.c;

import zen.ast.ZArrayLiteralNode;
import zen.ast.ZCatchNode;
import zen.ast.ZClassNode;
import zen.ast.ZErrorNode;
import zen.ast.ZFieldNode;
import zen.ast.ZFuncCallNode;
import zen.ast.ZFunctionNode;
import zen.ast.ZGetIndexNode;
import zen.ast.ZGetNameNode;
import zen.ast.ZGetterNode;
import zen.ast.ZGlobalNameNode;
import zen.ast.ZInstanceOfNode;
import zen.ast.ZLetNode;
import zen.ast.ZMapLiteralNode;
import zen.ast.ZMethodCallNode;
import zen.ast.ZNewObjectNode;
import zen.ast.ZNode;
import zen.ast.ZParamNode;
import zen.ast.ZSetIndexNode;
import zen.ast.ZSetNameNode;
import zen.ast.ZSetterNode;
import zen.ast.ZThrowNode;
import zen.ast.ZTryNode;
import zen.ast.ZVarNode;
import zen.lang.ZenTypeSafer;
import zen.parser.ZLogger;
import zen.parser.ZSourceEngine;
import zen.parser.ZSourceGenerator;
import zen.type.ZClassField;
import zen.type.ZClassType;
import zen.type.ZFunc;
import zen.type.ZFuncType;
import zen.type.ZGenericType;
import zen.type.ZType;
import zen.util.LibZen;
import zen.util.Var;

public class CGenerator extends ZSourceGenerator {

	public CGenerator() {
		super("c", "C99");
		this.TrueLiteral  = "1/*true*/";
		this.FalseLiteral = "0/*false*/";
		this.NullLiteral  = "NULL";

		this.TopType = "ZObject *";
		this.SetNativeType(ZType.BooleanType, "int");
		//		this.SetNativeType(ZType.IntType, "long long int");
		this.SetNativeType(ZType.IntType, "long");
		this.SetNativeType(ZType.FloatType, "double");
		this.SetNativeType(ZType.StringType, "const char *");

		this.SetMacro("assert", "assert($[0])", ZType.VoidType, ZType.BooleanType, ZType.StringType);
		this.SetMacro("print", "puts($[0])", ZType.VoidType, ZType.StringType);
		this.SetMacro("println", "puts($[0]); puts(\"\\n\")", ZType.VoidType, ZType.StringType);

		// Converter
		this.SetConverterMacro("(double)($[0])", ZType.FloatType, ZType.IntType);
		this.SetConverterMacro("(long)($[0])", ZType.IntType, ZType.FloatType);
		this.SetConverterMacro("LibZen_BooleanToString($[0])", ZType.StringType, ZType.BooleanType);
		this.SetConverterMacro("LibZen_IntToString($[0])", ZType.StringType, ZType.IntType);
		this.SetConverterMacro("LibZen_FloatToString($[0])", ZType.StringType, ZType.FloatType);

		// String
		this.SetMacro("+", "LibZen_StrCat($[0], $[1])", ZType.StringType, ZType.StringType, ZType.StringType);
		this.SetMacro("size", "strlen($[0])", ZType.IntType, ZType.StringType);
		this.SetMacro("substring", "LibZen_SubString($[0], $[1])", ZType.StringType, ZType.StringType, ZType.IntType);
		this.SetMacro("substring", "LibZen_SubString2($[0], $[1], $[2])", ZType.StringType, ZType.StringType, ZType.IntType, ZType.IntType);
		this.SetMacro("indexOf", "LibZen_IndexOf($[0], $[1])", ZType.IntType, ZType.StringType, ZType.StringType);
		this.SetMacro("indexOf", "LibZen_IndexOf2($[0], $[1], $[2])", ZType.IntType, ZType.StringType, ZType.StringType, ZType.IntType);
		this.SetMacro("equals", "LibZen_EqualsString($[0], $[1])", ZType.BooleanType, ZType.StringType, ZType.StringType);
		this.SetMacro("startsWith", "LibZen_StartsWith($[0], $[1])", ZType.BooleanType, ZType.StringType, ZType.StringType);
		this.SetMacro("endsWith", "LibZen_EndWidth($[0], $[1])", ZType.BooleanType, ZType.StringType, ZType.StringType);

		// Array
		this.SetMacro("size", "LibZen_ArraySize($[0])", ZType.IntType, ZGenericType._ArrayType);
		this.SetMacro("clear", "LibZen_ArrayClear($[0], $[1])", ZType.VoidType, ZGenericType._ArrayType, ZType.IntType);
		this.SetMacro("add", "LibZen_ArrayAdd($[0], $[1])", ZType.VoidType, ZGenericType._ArrayType, ZType.VarType);
		this.SetMacro("add", "LibZen_ArrayAdd2($[0], $[1], $[2])", ZType.VoidType, ZGenericType._ArrayType, ZType.IntType, ZType.VarType);

		this.HeaderBuilder.Append("#include<stdio.h>\n");
		this.HeaderBuilder.Append("#include<stdlib.h>\n");
		this.HeaderBuilder.Append("#include<assert.h>\n", "\n");

	}

	@Override public ZSourceEngine GetEngine() {
		return new ZSourceEngine(new ZenTypeSafer(this), this);
	}


	@Override protected void GenerateCode(ZType ContextType, ZNode Node) {
		if(Node.IsUntyped() && !Node.IsErrorNode() && !(Node instanceof ZGlobalNameNode)) {
			this.CurrentBuilder.Append("/*untyped*/" + this.NullLiteral);
			ZLogger._LogError(Node.SourceToken, "untyped error: " + Node);
		}
		else {
			if(ContextType != null && Node.Type != ContextType) {
				this.CurrentBuilder.Append("(");
				this.GenerateTypeName(ContextType);
				this.CurrentBuilder.Append(")");
			}
			Node.Accept(this);
		}
	}

	@Override public void VisitArrayLiteralNode(ZArrayLiteralNode Node) {
		@Var ZType ParamType = Node.Type.GetParamType(0);
		if(ParamType.IsIntType() || ParamType.IsBooleanType()) {
			this.CurrentBuilder.Append("LibZen_NewIntArray(");
		}
		else if(ParamType.IsFloatType()) {
			this.CurrentBuilder.Append("LibZen_NewFloatArray(");
		}
		else if(ParamType.IsStringType()) {
			this.CurrentBuilder.Append("LibZen_NewStringArray(");
		}
		else {
			this.CurrentBuilder.Append("LibZen_NewArray(");
		}
		this.CurrentBuilder.Append(String.valueOf(Node.GetListSize()));
		if(Node.GetListSize() > 0) {
			this.CurrentBuilder.Append(this.Camma);
		}
		this.VisitListNode("", Node, ")");
	}

	@Override public void VisitMapLiteralNode(ZMapLiteralNode Node) {
		@Var ZType ParamType = Node.Type.GetParamType(0);
		if(ParamType.IsIntType() || ParamType.IsBooleanType()) {
			this.CurrentBuilder.Append("LibZen_NewIntMap(");
		}
		else if(ParamType.IsFloatType()) {
			this.CurrentBuilder.Append("LibZen_NewFloatMap(");
		}
		else if(ParamType.IsStringType()) {
			this.CurrentBuilder.Append("LibZen_NewStringMap(");
		}
		else {
			this.CurrentBuilder.Append("LibZen_NewMap(");
		}
		this.CurrentBuilder.Append(String.valueOf(Node.GetListSize()));
		if(Node.GetListSize() > 0) {
			this.CurrentBuilder.Append(this.Camma);
		}
		this.VisitListNode("", Node, ")");
	}

	@Override public void VisitNewObjectNode(ZNewObjectNode Node) {
		this.CurrentBuilder.Append("_New"+this.NameClass(Node.Type));
		this.VisitListNode("(", Node, ")");
	}

	private String BaseName(ZType RecvType) {
		return RecvType.GetAsciiName(); // FIXME
	}

	@Override public void VisitGetIndexNode(ZGetIndexNode Node) {
		this.CurrentBuilder.Append(this.BaseName(Node.GetAstType(ZGetIndexNode._Recv)) + "GetIndex");
		this.CurrentBuilder.Append("(");
		this.GenerateCode(null, Node.AST[ZGetIndexNode._Index]);
		this.CurrentBuilder.Append(")");
	}

	@Override public void VisitSetIndexNode(ZSetIndexNode Node) {
		this.CurrentBuilder.Append(this.BaseName(Node.GetAstType(ZGetIndexNode._Recv)) + "SetIndex");
		this.CurrentBuilder.Append("(");
		this.GenerateCode(null, Node.AST[ZSetIndexNode._Index]);
		this.CurrentBuilder.Append(this.Camma);
		this.GenerateCode(null, Node.AST[ZSetIndexNode._Expr]);
		this.CurrentBuilder.Append(")");
	}

	@Override public void VisitGetNameNode(ZGetNameNode Node) {
		this.CurrentBuilder.Append(this.NameLocalVariable(Node.VarName, Node.VarIndex));
	}

	@Override public void VisitSetNameNode(ZSetNameNode Node) {
		this.CurrentBuilder.Append(this.NameLocalVariable(Node.VarName, Node.VarIndex));
		this.CurrentBuilder.AppendToken("=");
		this.GenerateCode(null, Node.AST[ZSetNameNode._Expr]);
	}

	@Override public void VisitGetterNode(ZGetterNode Node) {
		this.GenerateSurroundCode(Node.AST[ZGetterNode._Recv]);
		this.CurrentBuilder.Append("->");
		this.CurrentBuilder.Append(Node.FieldName);
	}

	@Override public void VisitSetterNode(ZSetterNode Node) {
		this.GenerateSurroundCode(Node.AST[ZSetterNode._Recv]);
		this.CurrentBuilder.Append("->");
		this.CurrentBuilder.Append(Node.FieldName);
		this.CurrentBuilder.AppendToken("=");
		this.GenerateCode(null, Node.AST[ZSetterNode._Expr]);
	}

	@Override public void VisitMethodCallNode(ZMethodCallNode Node) {
		//		this.GenerateSurroundCode(Node.AST[ZMethodCallNode._Recv]);
		//		this.CurrentBuilder.Append(".");
		//		this.CurrentBuilder.Append(Node.MethodName);
		//		this.VisitListNode("(", Node, ")");
	}

	@Override public void VisitFuncCallNode(ZFuncCallNode Node) {
		this.GenerateCode(null, Node.AST[ZFuncCallNode._Func]);
		this.VisitListNode("(", Node, ")");
	}

	//	@Override public void VisitCastNode(ZCastNode Node) {
	//		this.CurrentBuilder.Append("(");
	//		this.VisitType(Node.Type);
	//		this.CurrentBuilder.Append(")");
	//		this.GenerateSurroundCode(Node.AST[ZCastNode._Expr]);
	//	}

	@Override public void VisitThrowNode(ZThrowNode Node) {
		this.GenerateCode(null, Node.AST[ZThrowNode._Expr]);
		this.CurrentBuilder.Append("longjump(1)"); // FIXME
		this.CurrentBuilder.AppendWhiteSpace();
	}

	@Override public void VisitTryNode(ZTryNode Node) {
		//		this.CurrentBuilder.Append("try");
		//		this.GenerateCode(Node.AST[ZTryNode._Try]);
		//		if(Node.AST[ZTryNode._Catch] != null) {
		//			this.GenerateCode(Node.AST[ZTryNode._Catch]);
		//		}
		//		if (Node.AST[ZTryNode._Finally] != null) {
		//			this.CurrentBuilder.Append("finally");
		//			this.GenerateCode(Node.AST[ZTryNode._Finally]);
		//		}
	}

	@Override public void VisitCatchNode(ZCatchNode Node) {
		//		this.CurrentBuilder.Append("catch (");
		//		this.CurrentBuilder.Append(Node.ExceptionName);
		//		this.VisitTypeAnnotation(Node.ExceptionType);
		//		this.CurrentBuilder.Append(")");
		//		this.GenerateCode(Node.AST[ZCatchNode._Block]);
	}

	private String ParamTypeName(ZType Type) {
		if(Type.IsArrayType()) {
			return "ArrayOf" + this.ParamTypeName(Type.GetParamType(0));
		}
		if(Type.IsMapType()) {
			return "MapOf" + this.ParamTypeName(Type.GetParamType(0));
		}
		if(Type.IsFuncType()) {
			@Var String s = "FuncOf" + this.ParamTypeName(Type.GetParamType(0))+"Of";
			@Var int i = 0;
			while(i < Type.GetParamSize()) {
				s = s +  this.ParamTypeName(Type.GetParamType(i));
				i = i + 1;
			}
			return s;
		}
		if(Type.IsIntType()) {
			return "Int";
		}
		if(Type.IsFloatType()) {
			return "Float";
		}
		if(Type.IsVoidType()) {
			return "Void";
		}
		if(Type.IsVarType()) {
			return "Var";
		}
		return Type.ShortName;
	}

	private String GetCTypeName(ZType Type) {
		@Var String TypeName = null;
		if(Type.IsArrayType() || Type.IsMapType()) {
			TypeName = this.ParamTypeName(Type) + " *";
		}
		if(Type instanceof ZClassType) {
			TypeName = "struct " + this.NameClass(Type) + " *";
		}
		if(TypeName == null) {
			TypeName = this.GetNativeTypeName(Type);
		}
		return TypeName;
	}

	protected void GenerateFuncTypeName(ZType Type, String FuncName) {
		this.GenerateTypeName(Type.GetParamType(0));
		this.CurrentBuilder.Append(" (*" + FuncName + ")");
		@Var int i = 1;
		this.CurrentBuilder.Append("(");
		while(i < Type.GetParamSize()) {
			if(i > 1) {
				this.CurrentBuilder.Append(",");
			}
			this.GenerateTypeName(Type.GetParamType(i));
			i = i + 1;
		}
		this.CurrentBuilder.Append(")");
	}

	@Override protected void GenerateTypeName(ZType Type) {
		if(Type.IsFuncType()) {
			this.GenerateFuncTypeName(Type, "");
		}
		else {
			this.CurrentBuilder.Append(this.GetCTypeName(Type.GetRealType()));
		}
	}

	@Override public void VisitVarNode(ZVarNode Node) {
		this.GenerateTypeName(Node.DeclType);
		this.CurrentBuilder.Append(" ");
		this.CurrentBuilder.Append(this.NameLocalVariable(Node.NativeName, Node.VarIndex));
		this.CurrentBuilder.AppendToken("=");
		this.GenerateCode(null, Node.AST[ZVarNode._InitValue]);
		this.CurrentBuilder.Append(this.SemiColon);
		this.VisitStmtList(Node);
	}

	@Override public void VisitLetNode(ZLetNode Node) {
		if(Node.AST[ZLetNode._InitValue] instanceof ZErrorNode) {
			this.VisitErrorNode((ZErrorNode)Node.AST[ZLetNode._InitValue]);
			return;
		}
		this.CurrentBuilder.Append("static ");
		this.GenerateTypeName(Node.GetAstType(ZLetNode._InitValue));
		this.CurrentBuilder.Append(" ");
		this.CurrentBuilder.Append(Node.GlobalName);
		this.CurrentBuilder.Append(" = ");
		this.GenerateCode(null, Node.AST[ZLetNode._InitValue]);
		this.CurrentBuilder.Append(this.SemiColon);
		this.CurrentBuilder.AppendLineFeed();
		Node.GetNameSpace().SetLocalSymbol(Node.Symbol, Node.ToGlobalNameNode());
	}

	@Override public void VisitParamNode(ZParamNode Node) {
		if(Node.Type.IsFuncType()) {
			this.GenerateFuncTypeName(Node.Type, this.NameLocalVariable(Node.Name, Node.ParamIndex));
		}
		else {
			this.GenerateTypeName(Node.Type);
			this.CurrentBuilder.Append(" ");
			this.CurrentBuilder.Append(this.NameLocalVariable(Node.Name, Node.ParamIndex));
		}
	}

	@Override public void VisitFunctionNode(ZFunctionNode Node) {
		if(!Node.Type.IsVoidType()) {
			if(Node.FuncName == null) {
				Node.FuncName = "f";
			}
			@Var String FuncName = Node.FuncName + this.GetUniqueNumber();
			this.CurrentBuilder = this.InsertNewSourceBuilder();
			this.CurrentBuilder.Append("static ");
			this.GenerateTypeName(Node.ReturnType);
			this.CurrentBuilder.Append(" ");
			this.CurrentBuilder.Append(FuncName);
			this.VisitListNode("(", Node, ")");
			this.GenerateCode(null, Node.AST[ZFunctionNode._Block]);
			this.CurrentBuilder.AppendLineFeed();
			this.CurrentBuilder = this.CurrentBuilder.Pop();
			this.CurrentBuilder.Append(FuncName);
		}
		else {
			@Var int StartIndex = this.CurrentBuilder.GetPosition();
			this.CurrentBuilder.Append("static ");
			this.GenerateTypeName(Node.ReturnType);
			this.CurrentBuilder.Append(" ");
			this.CurrentBuilder.Append(Node.GetSignature(this));
			this.VisitListNode("(", Node, ")");
			@Var String Prototype = this.CurrentBuilder.CopyString(StartIndex, this.CurrentBuilder.GetPosition());
			this.GenerateCode(null, Node.AST[ZFunctionNode._Block]);
			this.CurrentBuilder.AppendLineFeed();
			this.CurrentBuilder.AppendLineFeed();
			this.HeaderBuilder.Append(Prototype);
			this.HeaderBuilder.Append(this.SemiColon);
			this.HeaderBuilder.AppendLineFeed();
			@Var ZFuncType FuncType = Node.GetFuncType(null);
			if(Node.IsExport) {
				this.GenerateExportFunction(Node);
			}
			if(this.IsMethod(Node.FuncName, FuncType)) {
				this.HeaderBuilder.Append("#define _" + this.NameMethod(FuncType.GetRecvType(), Node.FuncName));
				this.HeaderBuilder.AppendLineFeed();
			}
		}
	}

	private void GenerateExportFunction(ZFunctionNode Node) {
		if(Node.FuncName.equals("main")) {
			this.CurrentBuilder.Append("int");
		}
		else {
			this.GenerateTypeName(Node.ReturnType);
		}
		this.CurrentBuilder.Append(" ", Node.FuncName);
		this.VisitListNode("(", Node, ")");
		this.CurrentBuilder.AppendLineFeed();
		this.CurrentBuilder.Append("{");
		this.CurrentBuilder.Indent();
		this.CurrentBuilder.AppendLineFeedIndent();
		if(!Node.ReturnType.IsVoidType()) {
			this.CurrentBuilder.Append("return ");
		}
		this.CurrentBuilder.Append(Node.GetSignature(this));
		this.VisitListNode("(", Node, ")");
		this.CurrentBuilder.Append(this.SemiColon);
		if(Node.FuncName.equals("main")) {
			this.CurrentBuilder.AppendLineFeed();
			this.CurrentBuilder.Append("return 0;");
		}
		this.CurrentBuilder.UnIndent();
		this.CurrentBuilder.AppendLineFeed();
		this.CurrentBuilder.Append("}");
		this.CurrentBuilder.AppendLineFeed();
		this.CurrentBuilder.AppendLineFeed();
	}

	@Override public void VisitInstanceOfNode(ZInstanceOfNode Node) {
		this.CurrentBuilder.Append("LibZen_Is(");
		this.GenerateCode(null, Node.AST[ZInstanceOfNode._Left]);
		this.CurrentBuilder.Append(this.Camma);
		this.CurrentBuilder.AppendInt(Node.TargetType.TypeId);
		this.CurrentBuilder.Append(")");
	}

	private void GenerateCField(String CType, String FieldName) {
		this.CurrentBuilder.AppendLineFeed();
		this.CurrentBuilder.AppendIndent();
		this.CurrentBuilder.Append(CType);
		this.CurrentBuilder.AppendWhiteSpace();
		this.CurrentBuilder.Append(FieldName);
		this.CurrentBuilder.Append(this.SemiColon);
	}

	private void GenerateField(ZType DeclType, String FieldName) {
		this.CurrentBuilder.AppendLineFeedIndent();
		this.GenerateTypeName(DeclType);
		this.CurrentBuilder.AppendWhiteSpace();
		this.CurrentBuilder.Append(FieldName);
		this.CurrentBuilder.Append(this.SemiColon);
	}

	private void GenerateFields(ZClassType ClassType, ZType ThisType) {
		@Var ZType SuperType = ThisType.GetSuperType();
		if(!SuperType.IsVarType()) {
			this.GenerateFields(ClassType, SuperType);
		}
		@Var int i = 0;
		this.GenerateCField("int", "_classId" + ThisType.TypeId);
		this.GenerateCField("int", "_delta" + ThisType.TypeId);
		while (i < ClassType.GetFieldSize()) {
			@Var ZClassField ClassField = ClassType.GetFieldAt(i);
			if(ClassField.ClassType == ThisType) {
				this.GenerateField(ClassField.FieldType, ClassField.FieldName);
			}
			i = i + 1;
		}
	}

	@Override public void VisitClassNode(ZClassNode Node) {
		this.CurrentBuilder.Append("struct");
		this.CurrentBuilder.AppendWhiteSpace();
		this.CurrentBuilder.Append(this.NameClass(Node.ClassType));
		this.CurrentBuilder.AppendWhiteSpace();
		this.CurrentBuilder.Append("{");
		this.CurrentBuilder.Indent();
		this.GenerateFields(Node.ClassType, Node.ClassType);
		this.GenerateCField("int", "_nextId");
		this.CurrentBuilder.UnIndent();
		this.CurrentBuilder.AppendLineFeed();
		this.CurrentBuilder.AppendIndent();
		this.CurrentBuilder.Append("};");
		this.CurrentBuilder.AppendLineFeed();
		this.CurrentBuilder.AppendLineFeed();


		this.CurrentBuilder.Append("static void _Init" + this.NameClass(Node.ClassType));
		this.CurrentBuilder.Append("(");
		this.GenerateTypeName(Node.ClassType);
		this.CurrentBuilder.Append(" o) {");
		this.CurrentBuilder.Indent();
		@Var ZType SuperType = Node.ClassType.GetSuperType();
		if(!SuperType.IsVarType()) {
			this.CurrentBuilder.AppendLineFeedIndent();
			this.CurrentBuilder.Append("_Init" + this.NameClass(SuperType) + "((");
			this.GenerateTypeName(SuperType);
			this.CurrentBuilder.Append(")o);");
		}
		this.CurrentBuilder.AppendLineFeedIndent();
		this.CurrentBuilder.Append("o->_classId" + Node.ClassType.TypeId + " = " + Node.ClassType.TypeId + ";");
		this.CurrentBuilder.AppendLineFeedIndent();
		this.CurrentBuilder.Append("o->_delta" + Node.ClassType.TypeId + " = sizeof(struct " + this.NameClass(Node.ClassType)+ ") - ");
		if(SuperType.IsVarType()) {
			this.CurrentBuilder.Append("sizeof(int);");
		}
		else {
			this.CurrentBuilder.Append("sizeof(struct " + this.NameClass(SuperType) + ");");
		}
		@Var int i = 0;
		while (i < Node.GetListSize()) {
			@Var ZFieldNode FieldNode = Node.GetFieldNode(i);
			this.CurrentBuilder.AppendLineFeedIndent();
			this.CurrentBuilder.Append("o->");
			this.CurrentBuilder.Append(FieldNode.FieldName);
			this.CurrentBuilder.Append(" = ");
			if(FieldNode.DeclType.IsFuncType()) {
				this.CurrentBuilder.Append("NULL");
			}
			else {
				this.GenerateCode(null, FieldNode.AST[ZFieldNode._InitValue]);
			}
			this.CurrentBuilder.Append(this.SemiColon);
			i = i + 1;
		}

		i = 0;
		while (i < Node.ClassType.GetFieldSize()) {
			@Var ZClassField ClassField = Node.ClassType.GetFieldAt(i);
			if(ClassField.FieldType.IsFuncType()) {
				this.CurrentBuilder.AppendLineFeed();
				this.CurrentBuilder.Append("#ifdef _" + this.NameClass(Node.ClassType) + "_" + ClassField.FieldName);
				this.CurrentBuilder.AppendLineFeedIndent();
				this.CurrentBuilder.Append("o->");
				this.CurrentBuilder.Append(ClassField.FieldName);
				this.CurrentBuilder.Append(" = ");
				this.CurrentBuilder.Append(ZFunc._StringfySignature(ClassField.FieldName, ClassField.FieldType.GetParamSize()-1, Node.ClassType));
				this.CurrentBuilder.AppendLineFeed();
				this.CurrentBuilder.Append("#endif");
			}
			i = i + 1;
		}

		this.CurrentBuilder.AppendLineFeedIndent();
		this.CurrentBuilder.Append("o->_nextId = 0;");
		this.CurrentBuilder.UnIndent();
		this.CurrentBuilder.AppendLineFeedIndent();
		this.CurrentBuilder.Append("}");
		this.CurrentBuilder.AppendLineFeed();
		this.CurrentBuilder.AppendLineFeed();

		this.CurrentBuilder.Append("static ");
		this.GenerateTypeName(Node.ClassType);
		this.CurrentBuilder.Append(" _New" + this.NameClass(Node.ClassType));
		this.CurrentBuilder.Append("(void) {");
		this.CurrentBuilder.Indent();
		this.CurrentBuilder.AppendLineFeedIndent();
		this.GenerateTypeName(Node.ClassType);
		this.CurrentBuilder.Append("o = LibZen_Malloc(sizeof(struct " + this.NameClass(Node.ClassType) + "))");
		this.CurrentBuilder.Append(this.SemiColon);
		this.CurrentBuilder.AppendLineFeedIndent();
		this.CurrentBuilder.Append("_Init" + this.NameClass(Node.ClassType) + "(o);");
		this.CurrentBuilder.AppendLineFeedIndent();
		this.CurrentBuilder.Append("return o;");
		this.CurrentBuilder.UnIndent();
		this.CurrentBuilder.AppendLineFeedIndent();
		this.CurrentBuilder.Append("}");
		this.CurrentBuilder.AppendLineFeed();
		this.CurrentBuilder.AppendLineFeed();
	}

	@Override public void VisitErrorNode(ZErrorNode Node) {
		ZLogger._LogError(Node.SourceToken, Node.ErrorMessage);
		this.CurrentBuilder.Append("ThrowError(");
		this.CurrentBuilder.Append(LibZen._QuoteString(Node.ErrorMessage));
		this.CurrentBuilder.Append(")");
	}

	//	@Override public void VisitExtendedNode(ZNode Node) {
	//
	//	}




}