(use jaylib)

(set-config-flags :msaa-4x-hint)
(init-window 800 600 "hello box2d")
(set-target-fps 60)

# FIXME need to dig into src is a little weird
(import ./build/src/libjanetbox2d :as b2)

# 1. first create the world.
(def world (b2/create-world {:gravity [0 10]}))

# 2. then create a new body.
(def body (b2/create-body world
			  {:type :dynamic
			   :position [100 100]
			   :angle 0 }))

# 3. create a shape
(def ground-box (b2/make-box 50 10))

# 4. then add shape to body
(b2/create-polygon-shape body ground-box {:density 1 :friction 0.3})

# 5. step the physics world
(while (not (window-should-close))
  (b2/body-apply-force-to-center body [10000 0])
  (b2/world-step world (get-frame-time))

  (begin-drawing)
  (clear-background :black)
  (let [[x y] (body :position)]
    (draw-rectangle (math/round x)
		    (math/round y)
		    50 10 :red))
  (end-drawing))

(close-window)
