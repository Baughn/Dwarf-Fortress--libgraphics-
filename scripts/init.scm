(define (reload)
  (load "scripts/init.scm"))

(load "scripts/primitives.scm")


(define editor
  (make-ui
   



;; We want to wrap this up with a safe class-based interface in scheme, later.

(define input-shower #f)
(define (show-input)
  (define (feed l)
    (display l)
    (newline)
    #t)
  (set! input-shower
        (push-interface feed void void #t)))
(define (stop-showing-input)
  (del-interface input-shower))

(define clock-shower #f)
(define (show-clock)
  (define (render)
    (let ((now (current-date)))
      (set-color 'blue 'black #t)
      (addstr 40 0
              (format "~a:~a" (date-hour now) (date-minute now)))))
  (set! clock-shower (push-interface (const #t) void render #t)))
(define (stop-showing-clock)
  (del-interface clock-shower))
