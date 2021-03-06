// ***************************************************************************
// Copyright (c) 2013-2014, Konoha project authors. All rights reserved.
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

package zen.ast;

import zen.parser.ZToken;
import zen.parser.ZVisitor;
import zen.type.ZType;
import zen.util.Field;

// E.g., $NativeName = $ValueNode
public class ZSetNameNode extends ZNode {
	public final static int _Expr = 0;

	@Field public String  GivenName;
	@Field public int     VarIndex = 0;
	@Field public boolean IsCaptured = false;

	public ZSetNameNode(ZNode ParentNode, ZToken Token, String GivenName) {
		super(ParentNode, Token, 1);
		this.GivenName = GivenName;
	}

	public ZSetNameNode(String Name, ZNode ExprNode) {
		super(null, null, 1);
		this.GivenName = Name;
		this.SetNode(ZSetNameNode._Expr, ExprNode);
		if(!ExprNode.IsUntyped()) {
			this.Type = ZType.VoidType;
		}
	}

	public final String GetName() {
		return this.GivenName;
	}

	public final ZNode ExprNode() {
		return this.AST[ZSetNameNode._Expr ];
	}

	@Override public void Accept(ZVisitor Visitor) {
		Visitor.VisitSetNameNode(this);
	}
}