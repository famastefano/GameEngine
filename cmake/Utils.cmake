include_guard()

function(ge_copyLibrariesOnPostBuild tgt)
    
    if(ARGN LESS 2)
        message(FATAL_ERROR "At least 2 arguments are required: target + target dependency")
    endif()
    
    if(NOT TARGET ${tgt})
        message(FATAL_ERROR "${tgt} isn't a target.")
    endif()

    foreach(arg IN LISTS ARGN)
        if(NOT TARGET ${arg})
            message(FATAL_ERROR "${arg} isn't a target.")
        endif()
        add_custom_command(TARGET ${tgt}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${arg}> $<TARGET_FILE_DIR:${tgt}>
        )
    endforeach()

endfunction()