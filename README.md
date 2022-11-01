![gappa](/doc/logo/logo_readme.png?raw=true "gappa")

<!-- Genesis Applications for Phylogenetic Placement Analysis -->

[![Conda install](https://img.shields.io/conda/vn/bioconda/gappa)](https://anaconda.org/bioconda/gappa)
[![Downloads](https://img.shields.io/conda/dn/bioconda/gappa)](https://anaconda.org/bioconda/gappa)
[![Build Status](https://travis-ci.org/lczech/gappa.svg?branch=master)](https://travis-ci.org/lczech/gappa)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](http://www.gnu.org/licenses/gpl.html)
[![Softwipe Score](https://img.shields.io/badge/softwipe-9.0/10.0-blue)](https://github.com/adrianzap/softwipe/wiki/Code-Quality-Benchmark)
![Language](https://img.shields.io/badge/language-C%2B%2B11-lightgrey.svg)
[![Platforms](https://img.shields.io/conda/pn/bioconda/gappa)](https://anaconda.org/bioconda/gappa)

Features
-------------------

gappa is a collection of commands for working with phylogenetic data.
Its main focus are evolutionary placements of short environmental sequences on a reference phylogenetic tree.
Such data are typically produced by tools such as [EPA-ng](https://github.com/Pbdas/epa-ng),
[RAxML-EPA](http://sco.h-its.org/exelixis/web/software/epa/index.html) or
[pplacer](http://matsen.fhcrc.org/pplacer/), and usually stored in
[jplace](http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009) files.
<!-- It however also offers some commands for working with data such as sequences or trees. -->
See [**the Wiki pages**](https://github.com/lczech/gappa/wiki)
for the full list of all subcommands and their documentation.

We recommend our article ["Metagenomic Analysis Using Phylogenetic Placement — A Review of the First Decade"](https://doi.org/10.3389/fbinf.2022.871393) as an introduction to the topic of phylogenetic placement.

Setup
-------------------

There are two ways to get gappa:

1. Install it via [conda](https://anaconda.org/bioconda/gappa).
2. Build it from source.

While conda will only give you proper releases,
building from source will give you the latest (development) version.
For that, simply get it, and build it:

~~~.sh
git clone --recursive https://github.com/lczech/gappa.git
cd gappa
make
~~~

You can also use the green "Code" button above or
[click here](https://github.com/lczech/gappa/archive/master.zip) to download the source as a zip
archive. Unpack, and call `make` in the main directory to build everything.

Requirements:

 *  [Make](https://www.gnu.org/software/make/) and [CMake](https://cmake.org/) 2.8.12 or higher.
 *  A fairly up-to-date C++11 compiler, e.g.,
    [clang++ 3.6](http://clang.llvm.org/) or [GCC 4.9](https://gcc.gnu.org/), or higher.

After building, the executable is stored in the `bin` directory, and used as follows.

Usage and Documentation
-------------------

gappa is used via its command line interface, with subcommands for each task.
The commands have the general structure:

    gappa <module> <subcommand> <options>

<!-- The modules are simply a way of organizing the commands,
and have no [deeper meaning](https://en.wikipedia.org/wiki/42_%28answer%29). -->

See [**the Wiki pages**](https://github.com/lczech/gappa/wiki)
for the full list of all subcommands and their documentation.

For **bug reports and feature requests** of gappa, please
[open an issue on GitHub](https://github.com/lczech/gappa/issues).

For **user support**, please see our
[Phylogenetic Placement Google Group](https://groups.google.com/forum/#!forum/phylogenetic-placement).
It is intended for discussions about phylogenetic placement,
and for user support for our software tools, such as [EPA-ng](https://github.com/Pbdas/epa-ng),
[gappa](https://github.com/lczech/gappa), and [genesis](https://github.com/lczech/genesis).

Citation
-------------------

When using gappa commands, they will output the necessary citations for the command.

**To generally cite gappa, please use**

> Genesis and Gappa: processing, analyzing and visualizing phylogenetic (placement) data.<br />
> Lucas Czech, Pierre Barbera, and Alexandros Stamatakis.<br />
> Bioinformatics, 2020. https://doi.org/10.1093/bioinformatics/btaa070<br />

Furthermore, the command [`gappa tools citation`](https://github.com/lczech/gappa/wiki/Subcommand:-citation) can be used to obtain a full list of relevant references.

Many commands in gappa are implementations of our novel methods as described in

> Methods for Inference of Automatic Reference Phylogenies and Multilevel Phylogenetic Placement.<br />
> Lucas Czech, Pierre Barbera, and Alexandros Stamatakis.<br />
> Bioinformatics, 2018. https://doi.org/10.1093/bioinformatics/bty767<br />
> <!-- bioRxiv, 2018. https://doi.org/10.1101/299792 -->

and

> Scalable Methods for Analyzing and Visualizing Phylogenetic Placement of Metagenomic Samples.<br />
> Lucas Czech and Alexandros Stamatakis.<br />
> PLOS One, 2019. https://doi.org/10.1371/journal.pone.0217050<br />
> <!-- bioRxiv, 2019. https://doi.org/10.1101/346353 -->

At the same time, it offers some commands that are also implemented in the excellent
[guppy](http://matsen.github.io/pplacer/generated_rst/guppy.html) tool. When using these commands, please also cite

> pplacer: linear time maximum-likelihood and Bayesian phylogenetic placement of sequences onto a fixed reference tree.<br />
> Frederick A Matsen, Robin B Kodner, and Virginia Armbrust.<br />
> BMC Bioinformatics, 2010. https://doi.org/10.1186/1471-2105-11-538

However, being written in C++, our gappa is much faster and needs less memory for most of the tasks.

Furthermore, some methods build heavily on previous work of others. When using the KR distance computation, please cite

> The phylogenetic Kantorovich-Rubinstein metric for environmental sequence samples.<br />
> Steven Evans and Frederick Matsen.<br />
> Journal of the Royal Statistical Society, 2012. https://doi.org/10.1111/j.1467-9868.2011.01018.x

The Placement-Factorization heavily borrows from methods presented in

> Phylogenetic Factorization of Compositional Data Yields Lineage-Level Associations in Microbiome Datasets.<br />
> Alex Washburne, Justin Silverman, Jonathan Leff, Dominic Bennett, John Darcy, Sayan Mukherjee, Noah Fierer, and Lawrence David.<br />
> PeerJ, 2017. https://doi.org/10.7717/peerj.2969

The respetive commands in gappa also output information on which references need to be cited.

Lastly, we recommend reading our comprehensive review of the topic

> Metagenomic Analysis Using Phylogenetic Placement—A Review of the First Decade.<br />
> Lucas Czech, Alexandros Stamatakis, Micah Dunthorn, and Pierre Barbera.<br />
> Frontiers in Bioinformatics, 2022. https://doi.org/10.3389/fbinf.2022.871393  

to get an overview of phylogenetic placement and its methods.

Behind the scenes
-------------------

gappa is short for Genesis Applications for Phylogenetic Placement Analysis.
This is because most of the work of gappa is actually performed by our [genesis](https://github.com/lczech/genesis) library.
See there if you are interested in the implementation details.
