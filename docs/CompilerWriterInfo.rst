========================================================
Architecture & Platform Information for Compiler Writers
========================================================

.. contents::
   :local:

.. note::

  This document is a work-in-progress.  Additions and clarifications are
  welcome.

Hardware
========

AAP
---

* `AAP (An Altruistic Processor) Architecture Specification <https://www.embecosm.com/appnotes/ean13/ean13.pdf>`_

AArch64 & ARM
-------------

* `ARMv8-A Architecture Reference Manual <http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0487a.h/index.html>`_ (authentication required, free sign-up). This document covers both AArch64 and ARM instructions

* `ARMv7-M Architecture Reference Manual <http://infocenter.arm.com/help/topic/com.arm.doc.ddi0403e.b/index.html>`_ (authentication required, free sign-up). This covers the Thumb2-only microcontrollers

* `ARMv6-M Architecture Reference Manual <http://infocenter.arm.com/help/topic/com.arm.doc.ddi0419c/index.html>`_ (authentication required, free sign-up). This covers the Thumb1-only microcontrollers

* `ARM C Language Extensions <http://infocenter.arm.com/help/topic/com.arm.doc.ihi0053c/IHI0053C_acle_2_0.pdf>`_

* AArch32 `ABI Addenda and Errata <http://infocenter.arm.com/help/topic/com.arm.doc.ihi0045d/IHI0045D_ABI_addenda.pdf>`_

Itanium (ia64)
--------------

* `Itanium documentation <http://developer.intel.com/design/itanium2/documentation.htm>`_

Lanai
-----

* `Lanai Instruction Set Architecture <http://g.co/lanai/isa>`_


MIPS
----

* `MIPS Processor Architecture <https://www.mips.com/products/>`_

* `MIPS 64-bit ELF Object File Specification <http://techpubs.sgi.com/library/manuals/4000/007-4658-001/pdf/007-4658-001.pdf>`_

PowerPC
-------

IBM - Official manuals and docs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* `Power Instruction Set Architecture, Versions 2.03 through 2.06 (authentication required, free sign-up) <https://www.power.org/technology-introduction/standards-specifications>`_

* `PowerPC Compiler Writer's Guide <http://www.ibm.com/chips/techlib/techlib.nsf/techdocs/852569B20050FF7785256996007558C6>`_

* `Intro to PowerPC Architecture <http://www.ibm.com/developerworks/linux/library/l-powarch/>`_

* `PowerPC Processor Manuals (embedded) <http://www.ibm.com/chips/techlib/techlib.nsf/products/PowerPC>`_

* `Various IBM specifications and white papers <https://www.power.org/documentation/?document_company=105&document_category=all&publish_year=all&grid_order=DESC&grid_sort=title>`_

* `IBM AIX/5L for POWER Assembly Reference <http://publibn.boulder.ibm.com/doc_link/en_US/a_doc_lib/aixassem/alangref/alangreftfrm.htm>`_

Other documents, collections, notes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* `PowerPC ABI documents <http://penguinppc.org/dev/#library>`_
* `PowerPC64 alignment of long doubles (from GCC) <http://gcc.gnu.org/ml/gcc-patches/2003-09/msg00997.html>`_
* `Long branch stubs for powerpc64-linux (from binutils) <http://sources.redhat.com/ml/binutils/2002-04/msg00573.html>`_

AMDGPU
------

Refer to :doc:`AMDGPUUsage` for additional documentation.

RISC-V
------
* `RISC-V User-Level ISA Specification <https://riscv.org/specifications/>`_

SPARC
-----

* `SPARC standards <http://sparc.org/standards>`_
* `SPARC V9 ABI <http://sparc.org/standards/64.psabi.1.35.ps.Z>`_
* `SPARC V8 ABI <http://sparc.org/standards/psABI3rd.pdf>`_

SystemZ
-------

* `z/Architecture Principles of Operation (registration required, free sign-up) <http://www-01.ibm.com/support/docview.wss?uid=isg2b9de5f05a9d57819852571c500428f9a>`_

X86
---

* `AMD processor manuals <http://developer.amd.com/resources/developer-guides-manuals/>`_
* `Intel 64 and IA-32 manuals <http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html>`_
* `Intel Itanium documentation <http://www.intel.com/design/itanium/documentation.htm?iid=ipp_srvr_proc_itanium2+techdocs>`_
* `X86 and X86-64 SysV psABI <https://github.com/hjl-tools/x86-psABI/wiki/X86-psABI>`_
* `Calling conventions for different C++ compilers and operating systems  <http://www.agner.org/optimize/calling_conventions.pdf>`_

XCore
-----

* `The XMOS XS1 Architecture (ISA) <https://www.xmos.com/en/download/public/The-XMOS-XS1-Architecture%28X7879A%29.pdf>`_
* `Tools Development Guide (includes ABI) <https://www.xmos.com/download/public/Tools-Development-Guide%28X9114A%29.pdf>`_

Hexagon
-------

* `Hexagon Programmer's Reference Manuals and Hexagon ABI Specification (registration required, free sign-up) <https://developer.qualcomm.com/software/hexagon-dsp-sdk/tools>`_

Other relevant lists
--------------------

* `GCC reading list <http://gcc.gnu.org/readings.html>`_

ABI
===

* `System V Application Binary Interface <http://www.sco.com/developers/gabi/latest/contents.html>`_
* `Itanium C++ ABI <http://itanium-cxx-abi.github.io/cxx-abi/>`_

Linux
-----

* `Linux extensions to gabi <https://github.com/hjl-tools/linux-abi/wiki/Linux-Extensions-to-gABI>`_
* `PowerPC 64-bit ELF ABI Supplement <http://www.linuxbase.org/spec/ELF/ppc64/>`_
* `Procedure Call Standard for the AArch64 Architecture <http://infocenter.arm.com/help/topic/com.arm.doc.ihi0055a/IHI0055A_aapcs64.pdf>`_
* `ELF for the ARM Architecture <http://infocenter.arm.com/help/topic/com.arm.doc.ihi0044e/IHI0044E_aaelf.pdf>`_
* `ELF for the ARM 64-bit Architecture (AArch64) <http://infocenter.arm.com/help/topic/com.arm.doc.ihi0056a/IHI0056A_aaelf64.pdf>`_
* `System z ELF ABI Supplement <http://legacy.redhat.com/pub/redhat/linux/7.1/es/os/s390x/doc/lzsabi0.pdf>`_

OS X
----

* `Mach-O Runtime Architecture <http://developer.apple.com/documentation/Darwin/RuntimeArchitecture-date.html>`_
* `Notes on Mach-O ABI <http://www.unsanity.org/archives/000044.php>`_

Windows
-------

* `Microsoft PE/COFF Specification <http://www.microsoft.com/whdc/system/platform/firmware/pecoff.mspx>`_

NVPTX
=====

* `CUDA Documentation <http://docs.nvidia.com/cuda/index.html>`_ includes the PTX
  ISA and Driver API documentation

Miscellaneous Resources
=======================

* `Executable File Format library <http://www.nondot.org/sabre/os/articles/ExecutableFileFormats/>`_

* `GCC prefetch project <http://gcc.gnu.org/projects/prefetch.html>`_ page has a
  good survey of the prefetching capabilities of a variety of modern
  processors.
