(declare-project
  :name "janet-box2d"
  :description "Janet bindings to Box2d"
  :url ""
  :author "Alec Troemel <alectroemel@hotmail.com>")

(defn get-files [extension path]
  (->> (os/dir path)
       (filter |(peg/match ~{:main (* (some (+ :w "_")) ,extension (! "make") (! "~"))} $))
       (map |(string/format "%s%s" path $))))

(def get-headers (partial get-files ".h"))
(def get-source (partial get-files ".c"))

(declare-native
  :name "janet-box2d"
  :cflags [;default-cflags "-std=c17" "-mavx" "-Ibox2d/include" "-Ibox2d/extern/simde"]
  :lflags [;default-lflags "-lX11" ]
  :headers [;(get-headers "src/")
	    ;(get-headers "box2d/include/box2d/")
	    ;(get-headers "box2d/src/")]
  :source [;(get-source "src/")
	   ;(get-source "box2d/src/")])
