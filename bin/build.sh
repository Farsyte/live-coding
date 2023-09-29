#!/bin/bash -

cat <<EOF

     === === === === === === === === === === === === === === ===
                            STARTING BUILD
                   $(date)
     === === === === === === === === === === === === === === ===

EOF

if make -k -j ECHO=true loop
then bs="COMPLETED"
else bs="FAILED"
fi


cat <<EOF

     --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                           BUILD $bs
                   $(date)
     --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

             waiting for next update that needs a build.

EOF

exec make wait
