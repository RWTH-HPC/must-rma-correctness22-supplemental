add_compile_options("-g" "-fsanitize=thread")
set (linkerFlags "-Wl,--whole-archive ${CMAKE_BINARY_DIR}/lib/libonReportLoader.a -Wl,--no-whole-archive -ldl -fsanitize=thread")

addHybridTest ("MpiTSanBcast" "bcast.c" 2
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )

addHybridTest ("MpiTSanIbcast" "ibcast.c" 2
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )

addHybridTest ("MpiTSanRecv" "recv.c" 2
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )

addHybridTest ("MpiTSanIrecv" "irecv.c" 2
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )

addHybridTest ("MpiTSanScatter" "scatter.c" 2
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )

addHybridTest ("MpiTSanIscatter" "iscatter.c" 2
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )

addHybridTest ("MpiTSanIsendSingle" "isend_single.c" 2
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )

addHybridTest ("MpiTSanIsend" "isend.c" 2
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )

addHybridTest ("MpiTSanSend" "send.c" 2 
    "data race"
    ""
    "-fsanitize=thread"
    ${linkerFlags}
    )