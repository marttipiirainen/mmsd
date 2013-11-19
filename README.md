mmsd
====

Integrating MMS daemon ( http://git.kernel.org/pub/scm/network/ofono/mmsd.git ) into Mer / Nemomobile.

To get started, clone the repo, then:
  cd mmsd
  git submodule init
  git submodule update
  cd mmsd
  mb build -t mer-target-armv7hl ../rpm/mmsd.spec
