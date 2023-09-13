#!/bin/bash -

cat <<EOF

     === === === === === === === === === === === === === === ===
                            STARTING BUILD
                   $(date)
     === === === === === === === === === === === === === === ===

EOF

if make -kj loop
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
