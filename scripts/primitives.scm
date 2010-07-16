
(define (set-color fg bg bright)
  (define index (enum-set-indexer (make-enumeration '(black blue green cyan red magenta brown gray))))
  (set-color% (index fg) (index bg) (if bright 1 0)))
