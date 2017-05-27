# JSock Build Configuration
###########################

# Jsock Static Library
$(call slib, jsock)

# Chat Client Demo
$(call exec, demos/chat/client, \
    jsock,                      \
    $(call includes,            \
        source                  \
    ),                          \
    $(call libraries,           \
        ncurses                 \
    )                           \
)
