(declare-project
  :name "janet-box2d"
  :description "Janet bindings to Box2c (ie box2d version 3)"
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
  :cflags [;default-cflags "-std=c17" "-mavx" "-Ibox2c/include" "-Ibox2c/extern/simde"]
  :lflags [;default-lflags "-lX11" ]
  :headers [;(get-headers "src/")
	    ;(get-headers "box2c/include/box2d/")
	    ;(get-headers "box2c/src/")]
  :source [;(get-source "src/")
	   ;(get-source "box2c/src/")])
