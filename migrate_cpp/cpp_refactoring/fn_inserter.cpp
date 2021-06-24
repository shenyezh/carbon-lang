// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "migrate_cpp/cpp_refactoring/fn_inserter.h"

#include "clang/ASTMatchers/ASTMatchers.h"

namespace cam = ::clang::ast_matchers;

namespace Carbon {

FnInserter::FnInserter(std::map<std::string, Replacements>& in_replacements,
                       cam::MatchFinder* finder)
    : Matcher(in_replacements) {
  finder->addMatcher(
      cam::functionDecl(cam::anyOf(cam::hasTrailingReturn(),
                                   cam::returns(cam::asString("void"))),
                        cam::unless(cam::anyOf(cam::cxxConstructorDecl(),
                                               cam::cxxDestructorDecl())))
          .bind(Label),
      this);
}

void FnInserter::run(const cam::MatchFinder::MatchResult& result) {
  const auto* decl = result.Nodes.getNodeAs<clang::FunctionDecl>(Label);
  if (!decl) {
    llvm::report_fatal_error(std::string("getNodeAs failed for ") + Label);
  }
  auto begin = decl->getBeginLoc();
  // Replace the first token in the range, `auto`.
  auto range = clang::CharSourceRange::getTokenRange(begin, begin);
  AddReplacement(*(result.SourceManager), range, "fn");
}

}  // namespace Carbon