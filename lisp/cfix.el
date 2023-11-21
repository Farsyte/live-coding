(defun fix-comment () 
  "right-justify next comment in 64 bytes"
  (interactive)
  (beginning-of-line)
  (re-search-forward " ( [^)]*) *$")
  (end-of-line)
  (just-one-space)
  (beginning-of-line)
  (search-forward " ( ")
  (backward-char 2)
  (just-one-space)
  (end-of-line)
  (pad-to-column 65)
  (search-backward ")")
  (forward-char)
  (push-mark)
  (end-of-line)
  (kill-region (region-beginning) (region-end))
  (pop-mark)
  (beginning-of-line)
  (search-forward " ( ")
  (backward-char 2)
  (yank)
  (backward-char 1)
  (delete-char 1)
  (end-of-line)
  (forward-char)
  )