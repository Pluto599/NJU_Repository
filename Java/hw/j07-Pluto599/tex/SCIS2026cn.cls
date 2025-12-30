\def\sci@fileversion{4.01}
\def\sci@filedate{2025/11/18}
\def\@journal{中国科学: 信息科学}
\def\@issn{1674-7267}
\def\@copyright{$\copyright$~2026~}
\def\@website{\url{www.scichina.com}~~~~\url{infocn.scichina.com}}

\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{SCIS2026cn}[\sci@filedate, \sci@fileversion]

\let\CCTCJKfonts=1
\LoadClass[twoside,CJK]{ctexart}
%% ziti
\RequirePackage{xeCJK}
%\setCJKfamilyfont{zhsong}{simsun.ttc}
\setCJKfamilyfont{zhhei}{simhei.ttf}
%% math packages
\RequirePackage{amsmath,amsthm,amsfonts,amssymb,bm,upgreek,multicol,mathrsfs,pifont,amscd,latexsym,geometry,color,fancyhdr}
%% graphics packages
\RequirePackage{graphicx,picins}
%% other packages
\RequirePackage{algorithm,algorithmic,multirow,tabularx,url,float,booktabs}
%% check pdfTeX mode
\RequirePackage{ifpdf}
\RequirePackage{fancyhdr,lastpage}
\RequirePackage{supertabular}
\RequirePackage[pdfstartview=FitH,colorlinks,breaklinks,linkcolor=black,citecolor=black,filecolor=black,urlcolor=black,hyperindex,CJKbookmarks]{hyperref}

\RequirePackage{titlesec}%章节号
\urlstyle{same}%定义url字体和正文相同
\renewcommand{\today}{\number\year-\number\month-\number\day}%日期格式

%-------------------------- Page layout ------------------------------
\paperwidth=21truecm
\paperheight=28truecm
\textwidth=176truemm
\textheight=230truemm
\topmargin=-26pt
\headsep=6truemm
\oddsidemargin=-8truemm
\evensidemargin=-8truemm
\parindent=22pt
\footskip=10mm
\renewcommand{\baselinestretch}{1.3}
\renewcommand{\arraystretch}{1.2}
\newcommand{\sanhao}{\fontsize{18pt}{\baselineskip}\selectfont}
\newcommand{\xiaosanhao}{\fontsize{15pt}{2\baselineskip}\selectfont}
\newcommand{\xiaosihao}{\fontsize{12pt}{\baselineskip}\selectfont}
\newcommand{\wuhao}{\fontsize{10pt}{\baselineskip}\selectfont}
\newcommand{\jiuhao}{\fontsize{9pt}{\baselineskip}\selectfont}
\newcommand{\bahao}{\fontsize{8pt}{\baselineskip}\selectfont}
\newcommand{\qihao}{\fontsize{7pt}{0.8\baselineskip}\selectfont}
\newcommand{\liuhao}{\fontsize{6pt}{0.8\baselineskip}\selectfont}
\renewcommand{\textbf}[1]{{\bfseries\heiti #1}}
\definecolor{sciscolor}{rgb}{0.01,0.12,0.67}
%-------------------- Article Information ----------------------------
\let\@Luntan\@empty \def\Luntan#1{\def\@Luntan{#1}}
\let\@ArticleType\@empty \def\ArticleType#1{\def\@ArticleType{#1}}
\let\@SpecialTopic\@empty \def\SpecialTopic#1{\def\@SpecialTopic{#1}}
\let\@Citation\@empty \def\Citation#1{\def\@Citation{#1}}
\let\@Year\@empty \def\Year#1{\def\@Year{#1}}
\let\@Vol\@empty \def\Vol#1{\def\@Vol{#1}}
\let\@No\@empty \def\No#1{\def\@No{#1}}
\let\@AuthorMark\@empty \def\AuthorMark#1{\def\@AuthorMark{#1}}
\let\@AuthorCitation\@empty \def\AuthorCitation#1{\def\@AuthorCitation{#1}}
\let\@enAuthorCitation\@empty \def\enAuthorCitation#1{\def\@enAuthorCitation{#1}}
\let\@BeginPage\@empty \def\BeginPage#1{\def\@BeginPage{#1}}
\let\@DOI\@empty \def\DOI#1{\def\@DOI{#1}}
\let\@ReceiveDate\@empty \def\ReceiveDate#1{\def\@ReceiveDate{#1}}
\let\@ReviseDate\@empty \def\ReviseDate#1{\def\@ReviseDate{#1}}
\let\@AcceptDate\@empty \def\AcceptDate#1{\def\@AcceptDate{#1}}
\let\@OnlineDate\@empty \def\OnlineDate#1{\def\@OnlineDate{#1}}
\let\@Foundation\@empty \def\Foundation#1{\def\@Foundation{#1}}
\let\@comment\@empty \def\comment#1{\def\@comment{#1}}
\let\@encomment\@empty \def\encomment#1{\def\@encomment{#1}}
\let\@abstract\@empty \renewcommand\abstract[1]{\def\@abstract{#1}}
\let\@enabstract\@empty \def\enabstract#1{\def\@enabstract{#1}}
\let\@keywords\@empty \def\keywords#1{\def\@keywords{#1}}
\let\@enkeywords\@empty \def\enkeywords#1{\def\@enkeywords{#1}}
\let\@onlinetag\@empty
\let\@ReferenceName\@empty \def\ReferenceName#1{\def\@ReferenceName{#1}}
%-------------------------- Online ------------------------------------
\newcommand\online{
\def\@onlinetag{1}
\def\@BeginPage{1}
}

%-------------------------- Title ------------------------------------
\let\@title\@empty
\renewcommand\title[2]{
    \def\@title{#1}
    \def\@titlehead{#2}
    \def\@titlecitation{#2}
}

\let\@entitle\@empty
\newcommand\entitle[2]{
    \def\@entitle{#1}
    \def\@entitlecitation{#2}
}

\let\@author\@empty
\let\@enauthor\@empty
\let\@authoremaillist\@empty
\renewcommand*{\author}[4][]{
  \ifx\@author\@empty
    \def\@author{#2\@authornote{#1}{#4}}
    \def\@enauthor{#3\@enauthornote{#1}{#4}}
    \let\@enauthortemp\@enauthor
  \else
    \let\@enauthor\@enauthortemp
    \expandafter\def\expandafter\@author\expandafter{\@author,\,\, #2\@authornote{#1}{#4}}
    \expandafter\def\expandafter\@enauthor\expandafter{\@enauthor~{\&}~#3\@enauthornote{#1}{#4}}
    \expandafter\def\expandafter\@enauthortemp\expandafter{\@enauthortemp,~#3\@enauthornote{#1}{#4}}
  \fi
  \ifx#4\@empty\else
      \ifx\@authoremaillist\@empty \def\@authoremaillist{#4}
      \else \expandafter\def\expandafter\@authoremaillist\expandafter{\@authoremaillist,~#4}
      \fi
  \fi
  \let\@enauthoremaillist\@authoremaillist
}

\newcommand{\enauthoremaillist}[1]{
    \def\@enauthoremaillist{#1}
}


\def\@authornote#1#2{%
  \ifx#1\@empty
  \else \textsuperscript{\footnotesize #1}%
  \fi

  \ifx#2\@empty\else
  \ifx\@comment\@empty
  \textsuperscript{\hspace{-0.15mm}\bahao{*}}%
  \fi\fi
}

\def\@enauthornote#1#2{%
  \ifx#1\@empty
  \else \textsuperscript{#1}%
  \fi

  \ifx#2\@empty\else
  \ifx\@comment\@empty
  \textsuperscript{\hspace{-0.15mm}*}%
  \fi\fi
}

\def\@address{}
\newcommand*{\address}[3][]{
  \def\@addressnote{#1}
  \ifx\@address\@empty
    \ifx\@addressnote\@empty
        \def\@address{#2}
        \def\@enaddress{{\it #3}}
    \else
        \def\@address{#1.~#2}
        \def\@enaddress{#1.~{\it #3}}
    \fi
  \else
    \ifx\@addressnote\@empty
        \expandafter\def\expandafter\@address\expandafter{\@address\\{#2}}
        \expandafter\def\expandafter\@enaddress\expandafter{\@enaddress\\{\it #3}}
    \else
        \expandafter\def\expandafter\@address\expandafter{\@address\\{#1.~#2}}
        \expandafter\def\expandafter\@enaddress\expandafter{\@enaddress\\{#1.~{\it #3}}}
    \fi
  \fi
}


\renewcommand\maketitle{
\ifthenelse{\pageref{LastPage}=\@BeginPage}
    {\def\@pagerank{\@BeginPage}}
    {\def\@pagerank{\@BeginPage--\pageref{LastPage}}}


\ifthenelse{\equal{\@DOI}{\@empty}}
    {\def\@headtext{中国科学\,:\,信息科学\ \ 在审文章}}
    {\ifthenelse{\equal{\@onlinetag}{\@empty}}
    {\def\@headtext{中国科学\,:\,信息科学\ \ \@Year\,年\ \ 第\,\@Vol\,卷\ \ 第\,\@No\,期}}
    {\def\@headtext{中国科学\,:\,信息科学}}}

    %首页页眉页脚
    \begin{picture}(0,0)
        \put(-21,12){\parbox[c]{120mm}{\zihao{-5} SCIENTIA SINICA~\,\,Informationis}}
        \put(-21,30){\parbox[c]{120mm}{\jiuhao \@headtext onlinelatex}}
        \ifx\@DOI\@empty
            \put(-21,23){\color{sciscolor}{\rule{\linewidth}{0.5pt}}}
        \else
            \put(-21,23){\color{sciscolor}{\rule{0.76\linewidth}{0.5pt}}}
            \put(-21,12){\makebox[176truemm][r]{\includegraphics{SB-FC.eps}}}
            \put(-21,-14){\makebox[176truemm][r]{\href{http://crossmark.crossref.org/dialog/?doi=\@DOI&domain=pdf&date_stamp=\today}{\includegraphics{Print-CrossMark.eps}}}}
            \put(-21,-656){\makebox[176truemm][l]{\scriptsize\bf\heiti\@copyright《中国科学》杂志社}}
            \put(-21,-656){\makebox[176truemm][r]{\scriptsize\bf\heiti\@website}}
        \fi
        \ifx\@SpecialTopic\@empty
            \ifx\@ArticleType\@empty\else
                \put(-21,-7){\colorbox{sciscolor}{\heiti\color[rgb]{1,1,1}\zihao{-4}~~~\@ArticleType~~~}}
            \fi
        \else
            \ifx\@ArticleType\@empty
                \put(-21,-7){\colorbox{sciscolor}{\heiti\color[rgb]{1,1,1}\zihao{-4}~~~\@SpecialTopic~~~}}
            \else
                \put(-21,-7){\colorbox{sciscolor}{\heiti\color[rgb]{1,1,1}\zihao{-4}~~~\@SpecialTopic~{\bf\sanhao\raisebox{2pt}{.}}~\@ArticleType~~~}}
            \fi
        \fi
    \end{picture}

    \ifx\@Citation\@empty
        \ifx\@enAuthorCitation\@empty
            \begin{figure}[!b]
                \centering
                \colorbox[cmyk]{0,0,0,.15}{
                    \parbox[t]{159.6truemm{\bf\scriptsize\heiti 引用格式:\ }}
                    {\scriptsize \@AuthorCitation. \@titlecitation. 中国科学: 信息科学,
                    \ifx\@DOI\@empty{在审文章}\else{\@Year}{\ifx\@onlinetag\@empty{, \@Vol: \@pagerank}\fi}{, doi: }\href{https://doi.org/\@DOI}{\url{\@DOI}}\fi}}
            \end{figure}
        \else
            \begin{figure}[!b]
                \centering
                \colorbox[cmyk]{0,0,0,.15}{
                    \parbox[t]{159.6truemm{\bf\scriptsize\heiti 引用格式:\ }}
                    {\scriptsize \@AuthorCitation. \@titlecitation. 中国科学: 信息科学,
                    \ifx\@DOI\@empty{在审文章}\else{\@Year}{\ifx\@onlinetag\@empty{, \@Vol: \@pagerank}\fi}{, doi: }\href{https://doi.org/\@DOI}{\url{\@DOI}}\fi\\
                    \@enAuthorCitation. \@entitlecitation. Sci Sin Inform,
                    \ifx\@DOI\@empty{for review}\else{\@Year}{\ifx\@onlinetag\@empty{, \@Vol: \@pagerank}\fi}{, doi: }\href{https://doi.org/\@DOI}{\url{\@DOI}}\fi}}
            \end{figure}
        \fi
    \else
        \begin{figure}[!b]
            \centering
            \colorbox[cmyk]{0,0,0,.15}{
                \parbox[t]{159.6truemm{\bf\scriptsize\heiti 文章信息:\ }}
                {\scriptsize \@Citation}}
        \end{figure}
    \fi
    \renewcommand\thefootnote{\@fnsymbol\c@footnote}
    \def\@makefnmark{\hbox{\@textsuperscript{\normalfont\@thefnmark}}}
    \long\def\@makefntext##1{\noindent\@textsuperscript{\normalfont\@thefnmark}##1}
    \@maketitle
    \thispagestyle{empty}
    \long\def\@makefntext##1{\parindent 7mm \indent\hbox{\hss{\@thefnmark)\ }}\baselineskip 10pt ##1}
    \def\@makefnmark{\hbox\textsuperscript{\@thefnmark)}}
    \renewcommand\thefootnote{\arabic{footnote}}
    \setcounter{footnote}{0}
    \setcounter{page}{\@BeginPage}
    \setlength{\textheight}{234mm}
}

\renewcommand{\raggedright}{\leftskip=0pt \rightskip=0pt plus 0cm}
\def\@maketitle{
  \noindent
  \vspace{5mm}
    \begin{flushleft}
        %标题
        \renewcommand{\baselinestretch}{0.9}
        \bfseries\huge\heiti\zihao{2}\@title
    \end{flushleft}
    \ifx\@author\@empty
    \else
        \vskip 4 true mm
        \begin{flushleft}
            %作者
            \renewcommand{\baselinestretch}{0.9}
            {\rmfamily\Large\kaishu\zihao{4}\begin{tabular}[t]{@{}l@{}}\@author\end{tabular}}
        \end{flushleft}
        \vspace{-4mm}
        \begin{flushleft}
            \renewcommand{\baselinestretch}{1.35}
            \rmfamily\footnotesize\songti\zihao{6}
            %单位
            \@address\\
            %email
            *~通信作者. E-mail: \@authoremaillist
            %其他信息
            \ifx\@comment\@empty
            \else
                \\\@comment
            \fi
        \end{flushleft}
        \vspace{-4mm}
        \begin{flushleft}
            \renewcommand{\baselinestretch}{1.2}
            \rmfamily\footnotesize\songti\zihao{6}
            %日期
            \ifx\@DOI\@empty\else
            \ifx\@ReviseDate\@empty
                \ifx\@OnlineDate\@empty
                    收稿日期: \@ReceiveDate;\ \ 接受日期: \@AcceptDate\\
                \else
                    收稿日期: \@ReceiveDate;\ \ 接受日期: \@AcceptDate;\ \ 网络出版日期: \@OnlineDate\\
                \fi
            \else
                \ifx\@OnlineDate\@empty
                    收稿日期: \@ReceiveDate;\ \ 修回日期: \@ReviseDate;\ \ 接受日期: \@AcceptDate\\
                \else
                    收稿日期: \@ReceiveDate;\ \ 修回日期: \@ReviseDate;\ \ 接受日期: \@AcceptDate;\ \ 网络出版日期: \@OnlineDate\\
                \fi
            \fi\fi
        \end{flushleft}
        \ifx\@Foundation\@empty\vspace{-4mm}
        \else\vspace{-6mm}\hspace{-2.2mm}\begin{tabular}{p{\textwidth}!}\renewcommand{\baselinestretch}{1.2}\rmfamily\footnotesize\songti\zihao{6}\@Foundation\end{tabular}
        \fi
        \vspace{-4mm}
  \fi


  %摘要和关键词
  \ifx\@abstract\@empty
  \else
  \noindent
  \begin{picture}(0,0){\put(0,-5){\color{sciscolor}{\rule{\linewidth}{0.5pt}}}}\end{picture}
  {\vskip2.3mm\noindent{\heiti 摘要\quad\,\,}\ignorespaces\fangsong\@abstract\vspace{1mm}\\
  \noindent{\heiti 关键词\quad\,\,}\ignorespaces\fangsong\@keywords\\}
  \begin{picture}(0,0){\put(0,5){\color{sciscolor}{\rule{\linewidth}{0.5pt}}}}\end{picture}
  \fi
}


\def\makeentitle{
    \begin{flushleft}
        %英文标题
        \renewcommand{\baselinestretch}{0.85}
        \sloppy\bfseries\xiaosanhao\@entitle
    \end{flushleft}
    \ifx\@enauthor\@empty
    \else
        \begin{flushleft}
            %英文作者
            \renewcommand{\baselinestretch}{0.9}
            \normalsize
            \begin{tabular}[t]{@{}l@{}}\@enauthor\end{tabular}
        \end{flushleft}
        \begin{flushleft}
            \renewcommand{\baselinestretch}{0.7}
            \bahao
            %英文单位
            \sloppy\@enaddress\\
            %email
            *~Corresponding author. E-mail: \@enauthoremaillist
            %其他信息
            \ifx\@encomment\@empty
            \else
                \\\@encomment
            \fi
        \end{flushleft}
    \fi
    %英文摘要关键词
    \ifx\@enabstract\@empty
    \else
    \noindent\hspace*{-2.9mm}
    \begin{tabular}{p{\textwidth}}
    \renewcommand{\baselinestretch}{0.8}\jiuhao\textbf{Abstract}\quad\@enabstract\\
    \renewcommand{\baselinestretch}{0.8}\jiuhao\textbf{Keywords}\quad\@enkeywords
    \end{tabular}
    \fi
}

%----------------------- Running heads ---------------------------------
\pagestyle{fancy}
\lhead{}
\chead{\scriptsize\bf
        \ifx\@DOI\@empty \@headtext\ \ \ \thepage
        \else
            \ifx \@onlyoneauthor\@empty
                \@AuthorMark\ \ \ \@headtext\ \ \ \thepage
            \else
                \@AuthorMark{等}\ \ \ \@headtext\ \ \ \thepage
            \fi
        \fi}
\rhead{}
\lfoot{}
\cfoot{}
\rfoot{}
\renewcommand{\headrulewidth}{0.0pt}%改为0pt即可去掉页眉下面的横线
\renewcommand{\footrulewidth}{0.0pt}%改为0pt即可去掉页脚上面的横线



%-------------------------- Sections ----------------------------------
\newcommand*\Acknowledgements[1]{
    \vskip 5mm
    \baselineskip 16pt
    {\textbf{致谢}\quad{\fangsong#1}}\par
}

\newcommand*\Supplements[1]{
    \vskip 5mm
    \baselineskip 16pt
    {\textbf{补充材料}\quad{\fangsong#1~本文的补充材料见网络版infocn.scichina.com.
    补充材料为作者提供的原始数据, 作者对其学术质量和内容负责.}}\par
}
\titleformat{\section}{\large\bfseries\heiti}{\thesection}{1em}{}
\titleformat{\subsection}{\normalsize\bfseries\heiti}{\thesubsection}{1em}{}
\titleformat{\subsubsection}{\normalsize\bfseries\heiti}{\thesubsubsection}{1em}{}

\let\oldappendix\appendix
\renewenvironment{appendix}{
    \oldappendix
    \setcounter{section}{0}
    \def\thesection{\Alph{section}}
    \renewcommand{\theequation}{\Alph{section}\arabic{equation}}
    \renewcommand{\thefigure}{\Alph{section}\arabic{figure}}
    \renewcommand{\thetable}{\Alph{section}\arabic{table}}
    \renewcommand{\thealgorithm}{\Alph{section}\arabic{algorithm}}
    \renewcommand{\section}{
        \setcounter{equation}{0}
        \setcounter{table}{0}
        \setcounter{figure}{0}
        \setcounter{algorithm}{0}
        \@startsection{section}{1}{\z@}%
        {-2ex\@plus -2ex \@minus -.1ex}%
        {2ex \@plus .2ex}%
        {\normalfont\normalsize\bfseries}
    }
    \footnotesize
}

\newenvironment{supplement}{
    \newpage
    \oldappendix
    \setcounter{page}{1}
    \setcounter{section}{0}
    \setcounter{equation}{0}
    \setcounter{table}{0}
    \setcounter{figure}{0}
    \setcounter{algorithm}{0}
    \def\sectionname{补充材料(\thesection)}
    \def\thesection{\Roman{section}}
    \renewcommand{\theequation}{S\arabic{equation}}
    \renewcommand{\thefigure}{S\arabic{figure}}
    \renewcommand{\thetable}{S\arabic{table}}
    \renewcommand{\thealgorithm}{S\arabic{algorithm}}
    \renewcommand{\section}{
        \@startsection{section}{1}{\z@}%
        {-2ex\@plus -2ex \@minus -.1ex}%
        {2ex \@plus .2ex}%
        {\normalfont\normalsize\bfseries}
    }
    \footnotesize
}

%-------------------------- Bibliography ------------------------------
\def\@biblabel#1{#1~}

\renewenvironment{thebibliography}[1]{
    \vspace{6mm}\noindent
    \ifx\@ReferenceName\@empty
    \parbox[c]{20mm}{\heiti\zihao{5}参考文献}
    \else
    \parbox[c]{20mm}{\heiti\zihao{5}\@ReferenceName}
    \fi
    \hspace{-6mm}
    \vskip0.1in \baselineskip 11.5pt \vspace{-4mm}
    \list{\@biblabel{\@arabic\c@enumiv}}{
        \settowidth\labelwidth{\@biblabel{#1}}%
        \leftmargin\labelwidth
        \advance\leftmargin\labelsep
        \@openbib@code
        \usecounter{enumiv}%
        \let\p@enumiv\@empty
        \renewcommand\theenumiv{\@arabic\c@enumiv}
    }
    \itemsep 0ex \parskip 0ex
    \footnotesize\sloppy
}

\usepackage{cite}
\newcommand{\upcite}[1]{\hspace*{-4pt}\textsuperscript{\cite{#1}}}
\let\citedash\relax\makeatletter
\providecommand{\citedash}{\hbox{$\sim$}\penalty\@m}\makeatother

%------------------ Figures, tables, algorithms -----------------------

\setlength{\floatsep}{16pt plus 1pt minus 1pt}%页面顶部/底部的浮动对象间的垂直距离, 缺省为12pt plus 2pt minus 2pt
\setlength{\textfloatsep}{16pt plus 1pt minus 1pt}%页顶部/底部的浮动对象与文本间垂直距离, 缺省20pt plus 2pt minus 4pt
\setlength{\intextsep}{12pt plus 3pt minus 2pt}%页中浮动对象与上下方文本间垂直距离, 缺省12pt plus 2pt minus 2pt
\setlength{\belowrulesep}{0pt}%横线和其下文字的间距
\setlength{\aboverulesep}{0pt}%横线和其上文字的间距
\setlength{\doublerulesep}{0.4pt}
\usepackage[bf,footnotesize,labelsep=quad]{caption}
\captionsetup[table]{aboveskip=1mm}
\captionsetup[figure]{aboveskip=3mm}
\captionsetup[algorithm]{font=footnotesize}
\floatname{algorithm}{\heiti{算法}}

\newcommand*{\cnfigurename}{图}
\newcommand*{\enfigurename}{Figure}
\newcommand*{\cnenfigcaption}[2]{%
    \def\figurename{\jiuhao\bf\heiti\cnfigurename}
    \caption{\baselineskip 10pt\jiuhao\bf\heiti#1}
    \addtocounter{figure}{-1}
    \vspace*{-3mm}
    \def\figurename{\jiuhao\bf\heiti\enfigurename}
    \caption{\baselineskip 10pt\jiuhao\heiti\textnormal{#2}}
}

\newcommand*{\cntablename}{表}
\newcommand*{\entablename}{Table}
\newcommand*{\cnentablecaption}[2]{%
    \def\tablename{\jiuhao\bf\heiti\cntablename}
    \caption{\baselineskip 10pt\jiuhao\bf\heiti#1}
    \addtocounter{table}{-1}
    \vspace*{-1mm}
    \def\tablename{\jiuhao\bf\heiti\entablename}
    \caption{\baselineskip 10pt\jiuhao\heiti\textnormal{#2}}
}



%------------------------------ Math ----------------------------------
\def\leq{\leqslant}
\def\geq{\geqslant}
\def\le{\leqslant}
\def\ge{\geqslant}
\abovedisplayskip=11pt plus 0pt minus 6pt
\belowdisplayskip=11pt plus 0pt minus 6pt
\newcommand{\set}[1]{\ensuremath{\left\{ #1 \right\}}}%
\newcommand{\Set}[2]{\ensuremath{\left\{ #1 \left| \right. #2 \right\}}}%
\newcommand{\abs}[1]{\ensuremath{\left| #1 \right|}}%
\newcommand{\map}[3]{\ensuremath{#1 : #2 \rightarrow #3}}%
\newcommand{\C}[1]{\ensuremath{\mathcal{#1}}}%
\newcommand{\restrict}[2]{\ensuremath{#1 \left|_{#2}\right.}}%
\newcommand{\NN}{\mathbb{N}^0}
\newcommand{\PN}{\mathbb{N}}
\newcommand{\syn}{\mathbf{S}}
\newcommand{\Syn}{\mathbf{M}}
\allowdisplaybreaks
\setlength{\arraycolsep}{2.0pt}

%----------------------------- Theorems -------------------------------
\def\proof{{\heiti 证明}\quad}
\renewcommand{\qedsymbol}{} %去掉小方框
\newtheoremstyle{newdefinition}%
    {2pt}% Space above
    {2pt}% Space below
    {\normalfont}% Body font
    {2em}% Indent amount
    {\bfseries\heiti}% Theorem head font
    {}% Punctuation after theorem head
    {10pt plus 1pt minus 1pt}% Space after theorem head
    {}% Theorem head spec (can be left empty, meaning `normal')
\theoremstyle{newdefinition}
\newtheorem{definition}{定义}
\newtheorem{theorem}{定理}
\newtheorem{lemma}{引理}
\newtheorem{corollary}{推论}
\newtheorem{axiom}{公理}
\newtheorem{propsition}{命题}
\newtheorem{example}{例}
\newtheorem{exercise}{习题}
\newtheorem{solution}{解}
\newtheorem{notation}{注}
\newtheorem{assumption}{假设}
\newtheorem{remark}{注释}
\newtheorem{property}{性质}
\iffalse
\makeatletter
\renewcommand\normalsize{%
   \@setfontsize\normalsize\@xpt\@xiipt
   \abovedisplayskip 9\p@ \@plus4\p@ \@minus5\p@
   \abovedisplayshortskip \z@ \@plus3\p@
   \belowdisplayshortskip 6\p@ \@plus3\p@ \@minus3\p@
   \belowdisplayskip  \abovedisplayskip
   \let\@listi\@listI}
\makeatother
\fi

%--------------------------------- list ---------------------------------
\renewenvironment{itemize}{
\begin{list}{$\bullet$}{
\labelwidth=4em
\labelsep=0.5em
\leftmargin=0em
\rightmargin=0em
\parsep=\parskip
\itemsep=0em
\topsep=0em
\itemindent=2.1em
}
}{\end{list}}

\endinput
