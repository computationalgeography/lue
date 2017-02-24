
Variation through time: constant vs variable
Variation between time: same vs different

- item collection size
    - `constant_size`
    - `variable_size`
- time domain type
    - `omnipresent`
    - located
        - `shared`
        - `unique`
- value shape through time
    - `constant_shape`
    - `variable_shape`
- value shape per item
    - `same_shape`
    - `different_shape`


    constant_size
        time
            omnipresent
                same_shape
                different_shape
            shared
                constant_shape
                    same_shape
                    different_shape
                variable_shape
                    same_shape
                    different_shape
            unique
                ...
        space
            stationary
            mobile
    variable_size
        time
            shared
                ...
            unique
                ...
        space
            ...
