################################################################################
#
# riscv-openocd
#
################################################################################

RISCV_OPENOCD_VERSION = riscv
RISCV_OPENOCD_SITE = https://github.com/riscv/riscv-openocd.git
RISCV_OPENOCD_SITE_METHOD = git
RISCV_OPENOCD_GIT_SUBMODULES = YES
RISCV_OPENOCD_LICENSE = GPL-2.0+
RISCV_OPENOCD_LICENSE_FILES = COPYING

# Satisfy jimtcl autosetup
RISCV_OPENOCD_CONF_ENV = CXX=false

RISCV_OPENOCD_CONF_OPTS = \
	--enable-jtag_vpi \
	--enable-sysfsgpio \
	--enable-remote-bitbang \
	--disable-shared \
	--disable-werror

RISCV_OPENOCD_DEPENDENCIES = host-pkgconf

define RISCV_OPENOCD_CONFIGURE_CMDS
	( cd $(@D) && \
	PATH=$(BR_PATH) ./bootstrap nosubmodule && \
	rm -rf config.cache && \
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	$(RISCV_OPENOCD_CONF_ENV) \
	./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--sysconfdir=/etc \
		--localstatedir=/var \
		--program-prefix="" \
		$(RISCV_OPENOCD_CONF_OPTS) \
	)
endef

$(eval $(autotools-package))
