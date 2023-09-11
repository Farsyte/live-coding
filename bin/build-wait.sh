#!/bin/bash -

cat <<EOF

     === === === === === === === === === === === === === === ===
                            STARTING BUILD
                   $(date)
     === === === === === === === === === === === === === === ===

EOF

make -kj loop
make logs

cat <<EOF

     --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                           BUILD COMPLETED
                   $(date)
     --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

             waiting for next update that needs a build.

EOF

exec make wait
