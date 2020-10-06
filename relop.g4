lexer grammar relop;

tokens {
    Equal,
    NonEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    Other
}

Other:~[\r\n<>=]+;
Less:'<';
Greater:'>';
LessEqual:'<=';
Equal:'=';
NonEqual:'<>';
GreaterEqual:'>=';
WhiteSpace: [\r\n]+ -> skip;