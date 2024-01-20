# (use jaylib)

(import ./build/libjanetbox2d :as b2)


# (set-config-flags :msaa-4x-hint)
# (init-window 800 600 "hello box2d")
# (set-target-fps 60)

(def world (b2/create-world))
(def body (b2/create-body world [0 100] 0.3))

(b2/world-step world (/ 1 60))

# (while (not (window-should-close))
#   (begin-drawing)
#   (clear-background :black)
#   (end-drawing))

# (close-window)
