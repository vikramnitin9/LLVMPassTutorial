#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Lexer.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/CodeGen/CodeGenAction.h"

#include "llvm/IR/Metadata.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <unordered_set>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory FindFunctionCategory("");

class FunctionVisitor : public RecursiveASTVisitor<FunctionVisitor> {
public:
    explicit FunctionVisitor(ASTContext *_context, CompilerInstance &_compiler)
            : context(_context), compiler(_compiler) {}

    bool VisitFunctionDecl(FunctionDecl *function) {

        std::string functionName = function->getNameInfo().getName().getAsString();

        SourceLocation startLocation = function->getBeginLoc();
        SourceManager &SM = context->getSourceManager();
        if (!SM.isWrittenInMainFile(startLocation)) {
            startLocation = function->getLocation();  // Fallback to function name location
        }
        FullSourceLoc startLoc = context->getFullLoc(startLocation);
        FullSourceLoc endLoc = context->getFullLoc(function->getEndLoc());

        if (!startLoc.isValid() || !endLoc.isValid()) {
            return true;
        }

        if (SM.isInSystemHeader(startLoc)) {
            return true;
        }
        if (function->isThisDeclarationADefinition()) {
            int startLine = startLoc.getSpellingLineNumber();
            int endLine = endLoc.getSpellingLineNumber();
            int startCol = startLoc.getSpellingColumnNumber();
            int endCol = endLoc.getSpellingColumnNumber();

            // Get file name
            std::string fileName = SM.getFilename(startLocation).str();

            // Print everything
            std::cout << functionName << std::endl;
            std::cout << fileName << ":" << startLine << ":" << startCol << ":" << endLine << ":" << endCol << std::endl;
            std::cout << "====================" << std::endl;
        }
        return true;
    }

private:
    ASTContext *context;
    CompilerInstance &compiler;
};

class FunctionVisitorConsumer : public clang::ASTConsumer {
public:
    explicit FunctionVisitorConsumer(ASTContext *context,
                                     CompilerInstance &compiler)
            : visitor(context, compiler) {}

    virtual void HandleTranslationUnit(clang::ASTContext &context) {
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }

private:
    FunctionVisitor visitor;
};

class FunctionVisitAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer>
    CreateASTConsumer(clang::CompilerInstance &compiler, llvm::StringRef inFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new FunctionVisitorConsumer(&compiler.getASTContext(), compiler));
    }
};

int main(int argc, const char **argv) {
    auto expectedParser = CommonOptionsParser::create(argc, argv, FindFunctionCategory, llvm::cl::OneOrMore, "ast-visitor <source0> [... <sourceN>] --");
    if (!expectedParser) {
        llvm::errs() << expectedParser.takeError();
        return 1;
    }
    CommonOptionsParser& optionsParser = expectedParser.get();
    ClangTool tool(optionsParser.getCompilations(),
                   optionsParser.getSourcePathList());
    return tool.run(newFrontendActionFactory<FunctionVisitAction>().get());
}