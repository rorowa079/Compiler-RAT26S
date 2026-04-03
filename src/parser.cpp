#include "parser.h"
#include <stdexcept>
#include <iostream>

void Parser::parse() {
     advance(); 
    
     rat26s();
    
     if (currentToken.type != TokenType::END_OF_FILE) {
        syntaxError("Expected EOF but found additional tokens. ");
    } else {
        std::cout << "Parsing completed successfully." << std::endl;
    }
}

// R1. <Rat26S> ::= @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @
 void Parser::rat26s() {
    printProduction("<Rat26S> -> @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @");
    
    expect("@");
    optFunctionDefinitions();
    expect("@");
    optDeclarationList();
    expect("@");
    statementList();
    expect("@");
}

// R2. <Opt Function Definitions> ::= <Function Definitions> | <Empty>
 void Parser::optFunctionDefinitions() {
    printProduction("<Opt Function Definitions> -> <Function Definitions> | <Empty>");
    
     if (currentToken.lexeme == "function") {
        functionDefinitions();
    } 
 }

// R3. <Function Definitions> ::= <Function> | <Function> <Function Definitions>
 void Parser::functionDefinitions() {
    printProduction("<Function Definitions> -> <Function> <Function Definitions Prime>");
    function();
    
     if (currentToken.lexeme == "function") {
        functionDefinitions();
    }
}

// R4. <Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
 void Parser::function() {
    printProduction("<Function> -> function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>");
    
    expect("function");
    
     if (currentToken.type == TokenType::IDENTIFIER) {
        advance();
    } else {
        syntaxError("Expected an Identifier. ");
    }
    
    expect("(");
    optParameterList();
    expect(")");
    optDeclarationList();
    body();
}

// R5. <Opt Parameter List> ::= <Parameter List> | <Empty>
 void Parser::optParameterList() {
    printProduction("<Opt Parameter List> -> <Parameter List> | <Empty>");
    
     if (currentToken.type == TokenType::IDENTIFIER) {
        parameterList();
    }
 }

// R6. <Parameter List> ::= <Parameter> | <Parameter> , <Parameter List>
 void Parser::parameterList() {
    printProduction("<Parameter List> -> <Parameter> <Parameter List Prime>");
    
    parameter();
    
    if (currentToken.lexeme == ",") {
        advance();  
        parameterList();
    }
}

// R7. <Parameter> ::= <IDs> <Qualifier>
 void Parser::parameter() {
    printProduction("<Parameter> -> <IDs> <Qualifier>");
    ids();
    qualifier();
}

// R8. <Qualifier> ::= integer | boolean | real
 void Parser::qualifier() {
    printProduction("<Qualifier> -> integer | boolean | real");
    
    if (currentToken.lexeme == "integer" || 
        currentToken.lexeme == "boolean" || 
        currentToken.lexeme == "real") {
        advance();
    } else {
        syntaxError("Expected a Qualifier (integer, boolean, or real). ");
    }
}

// R9. <Body> ::= { <Statement List> }
 void Parser::body() {
    printProduction("<Body> -> { <Statement List> }");
    expect("{");
    statementList();
    expect("}");
}

// R13. <IDs> ::= <Identifier> | <Identifier> , <IDs>
 void Parser::ids() {
    printProduction("<IDs> -> <Identifier> <IDs Prime>");
    
    if (currentToken.type == TokenType::IDENTIFIER) {
        advance();
    } else {
        syntaxError("Expected an Identifier. ");
    }
    
    if (currentToken.lexeme == ",") {
        advance(); 
        ids();
    }
}

// R10. <Opt Declaration List> ::= <Declaration List> | <Empty>
 void Parser::optDeclarationList() {
    printProduction("<Opt Declaration List> -> <Declaration List> | <Empty>");
    
     if (currentToken.lexeme == "integer" || 
        currentToken.lexeme == "boolean" || 
        currentToken.lexeme == "real") {
        declarationList();
    }
 }

// R11. <Declaration List> ::= <Declaration> ; | <Declaration> ; <Declaration List>
  void Parser::declarationList() {
    printProduction("<Declaration List> -> <Declaration> ; <Declaration List Prime>");
    
    declaration();
    expect(";");
    
     if (currentToken.lexeme == "integer" || 
        currentToken.lexeme == "boolean" || 
        currentToken.lexeme == "real") {
        declarationList();
    }
}

// R12. <Declaration> ::= <Qualifier> <IDs>
 void Parser::declaration() {
    printProduction("<Declaration> -> <Qualifier> <IDs>");
    qualifier();
    ids();
}

// R14. <Statement List> ::= <Statement> | <Statement> <Statement List>
 void Parser::statementList() {
    printProduction("<Statement List> -> <Statement> <Statement List Prime>");
    
    statement();
    
     if (currentToken.lexeme == "{" || currentToken.type == TokenType::IDENTIFIER ||
        currentToken.lexeme == "if" || currentToken.lexeme == "return" ||
        currentToken.lexeme == "write" || currentToken.lexeme == "read" ||
        currentToken.lexeme == "while") {
        statementList();
    }
}

// R15. <Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>
 void Parser::statement() {
    printProduction("<Statement> -> <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>");
    
     if (currentToken.lexeme == "{") {
        compound();
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        assign();
    } else if (currentToken.lexeme == "if") {
        ifStatement();
    } else if (currentToken.lexeme == "return") {
        returnStatement();
    } else if (currentToken.lexeme == "write") {
        print();
    } else if (currentToken.lexeme == "read") {
        scan();
    } else if (currentToken.lexeme == "while") {
        whileStatement();
    } else {
        syntaxError("Expected a statement. ");
    }
}

// R16. <Compound> ::= { <Statement List> }
 void Parser::compound() {
    printProduction("<Compound> -> { <Statement List> }");
    expect("{");
    statementList();
    expect("}");
}

// R17. <Assign> ::= <Identifier> = <Expression> ;
 void Parser::assign() {
    printProduction("<Assign> -> <Identifier> = <Expression> ;");
    
    if (currentToken.type == TokenType::IDENTIFIER) {
        advance();
    } else {
        syntaxError("Expected an Identifier. ");
    }
    
    expect("=");
    expression();
    expect(";");
}

// R18. <If> ::= if ( <Condition> ) <Statement> <If Tail>
 void Parser::ifStatement() {
    printProduction("<If> -> if ( <Condition> ) <Statement> <If Tail>");
    expect("if");
    expect("(");
    condition();
    expect(")");
    statement();
    ifTail();
}

// R19. <If Tail> ::= fi | otherwise <Statement> fi
 void Parser::ifTail() {
    printProduction("<If Tail> -> fi | otherwise <Statement> fi");
    
    if (currentToken.lexeme == "fi") {
        advance();
    } else if (currentToken.lexeme == "otherwise") {
        advance();
        statement();
        expect("fi");
    } else {
        syntaxError("Expected 'fi' or 'otherwise'. ");
    }
}

// R20. <Return> ::= return <Return Tail>
 void Parser::returnStatement() {
    printProduction("<Return> -> return <Return Tail>");
    expect("return");
    returnTail();
}

// R21. <Return Tail> ::= ; | <Expression> ;
 void Parser::returnTail() {
    printProduction("<Return Tail> -> ; | <Expression> ;");
    
    if (currentToken.lexeme == ";") {
        advance();
    } else {
         expression();
        expect(";");
    }
}

// R22. <Print> ::= write ( <Expression> ) ;
 void Parser::print() {
    printProduction("<Print> -> write ( <Expression> ) ;");
    expect("write");
    expect("(");
    expression();
    expect(")");
    expect(";");
}

// R23. <Scan> ::= read ( <IDs> ) ;
 void Parser::scan() {
    printProduction("<Scan> -> read ( <IDs> ) ;");
    expect("read");
    expect("(");
    ids();
    expect(")");
    expect(";");
}

// R24. <While> ::= while ( <Condition> ) <Statement>
 void Parser::whileStatement() {
    printProduction("<While> -> while ( <Condition> ) <Statement>");
    expect("while");
    expect("(");
    condition();
    expect(")");
    statement();
}

// R25. <Condition> ::= <Expression> <Relop> <Expression>
 void Parser::condition() {
    printProduction("<Condition> -> <Expression> <Relop> <Expression>");
    expression();
    relop();
    expression();
}

// R26. <Relop> ::= == | != | > | < | <= | =>
 void Parser::relop() {
    printProduction("<Relop> -> == | != | > | < | <= | =>");
    if (currentToken.lexeme == "==" || currentToken.lexeme == "!=" ||
        currentToken.lexeme == ">" || currentToken.lexeme == "<" ||
        currentToken.lexeme == "<=" || currentToken.lexeme == "=>") {
        advance();
    } else {
        syntaxError("Expected a relational operator. ");
    }
}

// R27. <Expression> ::= <Term> <Expression Tail>
 void Parser::expression() {
    printProduction("<Expression> -> <Term> <Expression Tail>");
    term();
    expressionTail();
}

// R28. <Expression Tail> ::= + <Term> <Expression Tail> | - <Term> <Expression Tail> | <Empty>
 void Parser::expressionTail() {
    printProduction("<Expression Tail> -> + <Term> <Expression Tail> | - <Term> <Expression Tail> | <Empty>");
    
    if (currentToken.lexeme == "+" || currentToken.lexeme == "-") {
        advance();
        term();
        expressionTail();
    }
 }

// R29. <Term> ::= <Factor> <Term Tail>
 void Parser::term() {
    printProduction("<Term> -> <Factor> <Term Tail>");
    factor();
    termTail();
}

// R30. <Term Tail> ::= * <Factor> <Term Tail> | / <Factor> <Term Tail> | <Empty>
 void Parser::termTail() {
    printProduction("<Term Tail> -> * <Factor> <Term Tail> | / <Factor> <Term Tail> | <Empty>");
    
    if (currentToken.lexeme == "*" || currentToken.lexeme == "/") {
        advance();
        factor();
        termTail();
    }
 }

// R31. <Factor> ::= - <Primary> | <Primary>
 void Parser::factor() {
    printProduction("<Factor> -> - <Primary> | <Primary>");
    
    if (currentToken.lexeme == "-") {
        advance();
        primary();
    } else {
        primary();
    }
}

// R32. <Primary> ::= <Identifier> <Primary Tail> | <Integer> | ( <Expression> ) | <Real> | true | false
 void Parser::primary() {
    printProduction("<Primary> -> <Identifier> <Primary Tail> | <Integer> | ( <Expression> ) | <Real> | true | false");
    
    if (currentToken.type == TokenType::IDENTIFIER) {
        advance();
        primaryTail();
    } else if (currentToken.type == TokenType::INTEGER) {
        advance();
    } else if (currentToken.lexeme == "(") {
        advance();
        expression();
        expect(")");
    } else if (currentToken.type == TokenType::REAL) {
        advance();
    } else if (currentToken.lexeme == "true" || currentToken.lexeme == "false") {
        advance();
    } else {
        syntaxError("Expected a primary expression. ");
    }
}

// R33. <Primary Tail> ::= <Empty> | ( <IDs> )
 void Parser::primaryTail() {
    printProduction("<Primary Tail> -> <Empty
