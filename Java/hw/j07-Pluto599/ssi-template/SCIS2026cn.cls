\def\sci@fileversion{3.03}
\def\sci@filedate{2025/11/11}
\def\@journal{�й���ѧ: ��Ϣ��ѧ}
\def\@issn{1674-7267}
\def\@copyright{$\copyright$~2026~}
\def\@website{\url{www.scichina.com}~~~~\url{infocn.scichina.com}}

\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{SCIS2026cn}[\sci@filedate, \sci@fileversion]

\let\CCTCJKfonts=1
\LoadClass[twoside,CJK]{cctart}
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
\RequirePackage{breakurl}
%\RequirePackage[none]{hyphenat}%���ʲ�����
\RequirePackage{titlesec}%�½ں�
\urlstyle{same}%����url�����������ͬ
\renewcommand{\today}{\number\year-\number\month-\number\day}%���ڸ�ʽ

%-------------------------- Page layout ------------------------------
\paperwidth 21 true cm                  % ֽ�ſ�
\paperheight 28 true cm                 % ֽ�Ÿ�
\textwidth=176truemm
\textheight=230truemm
\topmargin=-26pt
\headsep=6truemm
%\oddsidemargin=1pt
%\evensidemargin=1pt
\oddsidemargin=-8truemm
\evensidemargin=-8truemm
\parindent=22pt
\footskip=10mm
\hyphenpenalty=10000%���õ��ʶϴʳͷ�
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
\definecolor{sciscolor}{rgb}{0.01,0.12,0.67}
%-------------------- Article Information ----------------------------
\let\@Luntan\@empty \def\Luntan#1{\def\@Luntan{#1}}
\let\@ZKYLuntan\@empty \def\ZKYLuntan#1{\def\@ZKYLuntan{#1}}
\let\@SqLuntan\@empty \def\SqLuntan#1{\def\@SqLuntan{#1}}
\let\@ArticleType\@empty \def\ArticleType#1{\def\@ArticleType{#1}}
\let\@Citation\@empty \def\Citation#1{\def\@Citation{#1}}
\let\@SpecialTopic\@empty \def\SpecialTopic#1{\def\@SpecialTopic{#1}}
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
\let\@onlyoneauthor\@empty
\renewcommand*{\author}[4][]{
  \ifx\@author\@empty
    \def\@author{#2\@authornote{#1}{#4}}
    \def\@enauthor{#3\@enauthornote{#1}{#4}}
    \let\@enauthortemp\@enauthor
  \else
    \def\@onlyoneauthor{No}
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
    {\def\@headtext{�й���ѧ\,:\,��Ϣ��ѧ\ \ ��������}}
    {\ifthenelse{\equal{\@onlinetag}{\@empty}}
    {\def\@headtext{�й���ѧ\,:\,��Ϣ��ѧ\ \ \@Year\,��\ \ ��\,\@Vol\,��\ \ ��\,\@No\,��}}
    {\def\@headtext{�й���ѧ\,:\,��Ϣ��ѧ}}}

    %��ҳҳüҳ��
    \begin{picture}(0,0)
        \put(-21,12){\parbox[c]{120mm}{\zihao{-5} SCIENTIA SINICA Informationis}}
        \put(-21,30){\parbox[c]{120mm}{\jiuhao \@headtext\ifx\@onlinetag\@empty{:~\@pagerank}\fi}}
        \ifx\@ZKYLuntan\@empty\else
            \put(-21,11){\makebox[134truemm][r]{\heiti\zihao{-5}\@ZKYLuntan}}
            \put(-21,-85){\makebox[176truemm][r]{\vbox{\hbox{\includegraphics{kxjslt.eps}}}}}
        \fi
        \ifx\@SqLuntan\@empty\else
            \put(-21,11){\makebox[134truemm][r]{\heiti\zihao{-5}\@SqLuntan}}
            \put(-21,-85){\makebox[176truemm][r]{\vbox{\hbox{\includegraphics{sqlt-SSC.eps}}}}}       
        \fi
        \ifx\@Luntan\@empty\else
            \put(-21,11){\makebox[134truemm][r]{\heiti\zihao{-5}\@Luntan}}
        \fi
        \ifx\@DOI\@empty
            \put(-21,23){\color{sciscolor}{\rule{\linewidth}{0.5pt}}}
            \else
            \put(-21,23){\color{sciscolor}{\rule{0.76\linewidth}{0.5pt}}}
            \put(-21,12){\makebox[176truemm][r]{\includegraphics{SB-FC.eps}}}
            \put(-21,-14){\makebox[176truemm][r]{\href{http://crossmark.crossref.org/dialog/?doi=\@DOI&domain=pdf&date_stamp=\today}{\includegraphics{Print-CrossMark.eps}}}}
            \put(-21,-656){\makebox[176truemm][l]{\scriptsize\bf\@copyright���й���ѧ����־��}}
            \put(-21,-656){\makebox[176truemm][r]{\scriptsize\bf\@website}}
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
                    \parbox[t]{159.6truemm{\bf\scriptsize ���ø�ʽ:\ }}
                    {\scriptsize \@AuthorCitation. \@titlecitation. �й���ѧ: ��Ϣ��ѧ,
                    \ifx\@DOI\@empty{��������}
                    \else{\@Year}{\ifx\@onlinetag\@empty{, \@Vol: \@pagerank}\fi}{, doi: }\href{https://doi.org/\@DOI}{\url{\@DOI}}
                    \fi}}
            \end{figure}
        \else
            \begin{figure}[!b]
                \centering
                \colorbox[cmyk]{0,0,0,.15}{
                    \parbox[t]{159.6truemm{\bf\scriptsize ���ø�ʽ:\ }}
                    {\scriptsize \@AuthorCitation. \@titlecitation. �й���ѧ: ��Ϣ��ѧ,
                    \ifx\@DOI\@empty{��������}
                    \else{\@Year}{\ifx\@onlinetag\@empty{, \@Vol: \@pagerank}\fi}{, doi: }\href{https://doi.org/\@DOI}{\url{\@DOI}}
                    \fi\\
                    \@enAuthorCitation. \@entitlecitation. Sci Sin Inform,
                    \ifx\@DOI\@empty{for review}
                    \else{\@Year}{\ifx\@onlinetag\@empty{, \@Vol: \@pagerank}\fi}{, doi: }\href{https://doi.org/\@DOI}{\url{\@DOI}}
                    \fi}}
            \end{figure}
        \fi
    \else
        \begin{figure}[!b]
            \centering
            \colorbox[cmyk]{0,0,0,.15}{
                \parbox[t]{159.6truemm{\bf\scriptsize ������Ϣ:\ }}
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
        %����
        \renewcommand{\baselinestretch}{0.9}
        \bfseries\huge\heiti\zihao{2}\@title
    \end{flushleft}
    \ifx\@author\@empty
    \else
        \vskip 4 true mm
        \begin{flushleft}
            %����
            \renewcommand{\baselinestretch}{0.9}
            {\rmfamily\Large\kaishu\zihao{4}\begin{tabular}[t]{@{}l@{}}\@author\end{tabular}}
        \end{flushleft}
        \vspace{-4mm}
        \begin{flushleft}
            \renewcommand{\baselinestretch}{1.35}
            \rmfamily\footnotesize\songti\zihao{6}
            %��λ
            \@address\\
            %email
            *~ͨ������. E-mail: \@authoremaillist
            %������Ϣ
            \ifx\@comment\@empty
            \else\\\@comment
            \fi
        \end{flushleft}
        \vspace{-4mm}
        \begin{flushleft}
            \renewcommand{\baselinestretch}{1.2}
            \rmfamily\footnotesize\songti\zihao{6}
            %����
            \ifx\@DOI\@empty\else
            \ifx\@ReceiveDate\@empty\else
            \ifx\@ReviseDate\@empty
                \ifx\@OnlineDate\@empty�ո�����: \@ReceiveDate;\ \ ��������: \@AcceptDate\\
                \else�ո�����: \@ReceiveDate;\ \ ��������: \@AcceptDate;\ \ �����������: \@OnlineDate\\
                \fi
            \else
                \ifx\@OnlineDate\@empty�ո�����: \@ReceiveDate;\ \ �޻�����: \@ReviseDate;\ \ ��������: \@AcceptDate\\
                \else�ո�����: \@ReceiveDate;\ \ �޻�����: \@ReviseDate;\ \ ��������: \@AcceptDate;\ \ �����������: \@OnlineDate\\
                \fi
            \fi\fi\fi
        \end{flushleft}
        \ifx\@Foundation\@empty\vspace{-4mm}
        \else\vspace{-6mm}\hspace{-2.2mm}\begin{tabular}{p{\textwidth}!}\renewcommand{\baselinestretch}{1.2}\rmfamily\footnotesize\songti\zihao{6}\@Foundation\end{tabular}
        \fi
        \vspace{-4mm}
  \fi


  %ժҪ�͹ؼ���
  \ifx\@abstract\@empty
  \else
  \noindent
  %\begin{picture}(0,0){\put(0,-5){\color[rgb]{0.4,0.4,0.4}\line(1,0){455}}}\end{picture}
  \begin{picture}(0,0){\put(0,-5){\color{sciscolor}{\rule{\linewidth}{0.5pt}}}}\end{picture}
  {\vskip2.3mm\noindent{\heiti ժҪ\quad\,\,}\ignorespaces\fangsong\@abstract\vspace{1mm}\\
  \noindent{\heiti �ؼ���\quad\,\,}\ignorespaces\fangsong\@keywords\\}
  \begin{picture}(0,0){\put(0,5){\color{sciscolor}{\rule{\linewidth}{0.5pt}}}}\end{picture}
  \fi
}


\def\makeentitle{
    \begin{flushleft}
        %Ӣ�ı���
        \renewcommand{\baselinestretch}{0.85}
        \sloppy\bfseries\xiaosanhao\@entitle
    \end{flushleft}
    \ifx\@enauthor\@empty
    \else
        \begin{flushleft}
            %Ӣ������
            \renewcommand{\baselinestretch}{0.9}
            \normalsize
            \begin{tabular}[t]{@{}l@{}}\@enauthor\end{tabular}
        \end{flushleft}
        \begin{flushleft}
            \renewcommand{\baselinestretch}{0.7}
            \bahao
            %Ӣ�ĵ�λ
            \sloppy\@enaddress\\
            %email
            *~Corresponding author. E-mail: \@enauthoremaillist
            %������Ϣ
            \ifx\@encomment\@empty
            \else
                \\\@encomment
            \fi
        \end{flushleft}
    \fi
    %Ӣ��ժҪ�ؼ���
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
                \@AuthorMark{��}\ \ \ \@headtext\ \ \ \thepage
            \fi
        \fi}
\rhead{}
\lfoot{}
\cfoot{}
\rfoot{}
\renewcommand{\headrulewidth}{0.0pt}%��Ϊ0pt����ȥ��ҳü����ĺ���
\renewcommand{\footrulewidth}{0.0pt}%��Ϊ0pt����ȥ��ҳ������ĺ���


%-------------------------- Sections ----------------------------------
\newcommand*\Acknowledgements[1]{
    \vskip 5mm
    \baselineskip 16pt
    {\textbf{��л}\quad{\fangsong#1}}\par
}

\newcommand*\Supplements[1]{
    \vskip 5mm
    \baselineskip 16pt
    {\textbf{�������}\quad{\fangsong#1~���ĵĲ�����ϼ������infocn.scichina.com.
    �������Ϊ�����ṩ��ԭʼ����, ���߶���ѧ�����������ݸ���.}}\par
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
    \def\sectionname{�������(\thesection)}
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
    \parbox[c]{20mm}{\heiti\zihao{5}�ο�����}
    \else
    \parbox[c]{20mm}{\heiti\zihao{5}\@ReferenceName}
    \fi
    \hspace{-6mm}
    %\rule[0.01\baselineskip]{0.9\textwidth}{0.5pt}
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

\setlength{\floatsep}{16pt plus 1pt minus 1pt}%ҳ�涥��/�ײ��ĸ��������Ĵ�ֱ����, ȱʡΪ12pt plus 2pt minus 2pt
\setlength{\textfloatsep}{16pt plus 1pt minus 1pt}%ҳ����/�ײ��ĸ����������ı��䴹ֱ����, ȱʡ20pt plus 2pt minus 4pt
\setlength{\intextsep}{12pt plus 3pt minus 2pt}%ҳ�и������������·��ı��䴹ֱ����, ȱʡ12pt plus 2pt minus 2pt
\setlength{\belowrulesep}{0pt}%���ߺ��������ֵļ��
\setlength{\aboverulesep}{0pt}%���ߺ��������ֵļ��
\setlength{\doublerulesep}{0.4pt}
\usepackage[bf,footnotesize,labelsep=quad]{caption}
\captionsetup[subfloat]{labelformat=simple,captionskip=0pt}
\captionsetup[table]{aboveskip=1mm}
\captionsetup[figure]{aboveskip=3mm}
\captionsetup[algorithm]{font=footnotesize}
\floatname{algorithm}{�㷨}

\newcommand*{\cnfigurename}{ͼ}
\newcommand*{\enfigurename}{Figure}
\newcommand*{\cnenfigcaption}[2]{%
    \def\figurename{\cnfigurename}
    \caption{\baselineskip 10pt\jiuhao\bf #1}
    \addtocounter{figure}{-1}
    \vspace*{-3mm}
    \def\figurename{\enfigurename}
    \caption{\baselineskip 10pt #2}
}

\newcommand*{\cntablename}{��}
\newcommand*{\entablename}{Table}
\newcommand*{\cnentablecaption}[2]{%
    \def\tablename{\cntablename}
    \caption{\baselineskip 10pt\jiuhao\bf #1}
    \addtocounter{table}{-1}
    \vspace*{-1mm}
    \def\tablename{\entablename}
    \caption{\baselineskip 10pt #2}
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
\def\proof{{\heiti ֤��}\quad}
\renewcommand{\qedsymbol}{} %ȥ��С����
\newtheoremstyle{newdefinition}%
    {2pt}% Space above
    {2pt}% Space below
    {\normalfont}% Body font
    {1.83em}% Indent amount
    {\bfseries}% Theorem head font
    {}% Punctuation after theorem head
    {10pt plus 1pt minus 1pt}% Space after theorem head
    {}% Theorem head spec (can be left empty, meaning `normal')
\theoremstyle{newdefinition}%
\newtheorem{definition}{����}
\newtheorem{theorem}{����}
\newtheorem{lemma}{����}
\newtheorem{corollary}{����}
\newtheorem{axiom}{����}
\newtheorem{propsition}{����}
\newtheorem{example}{��}
\newtheorem{exercise}{ϰ��}
\newtheorem{solution}{��}
\newtheorem{notation}{ע}
\newtheorem{assumption}{����}
\newtheorem{remark}{ע��}
\newtheorem{property}{����}
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
