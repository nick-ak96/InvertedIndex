# Algorithms of set-containment queries

This document covers a detailed desciption of the algorithms used for set-containment queries:

- *Set existence*
- *Subset existence*
- *All subset retrieval*
- *Set existence*
- *Superset existence*
- *All superset retrieval*

### Definitions and notations

* *S* - a set of sets that is stored in the index.
* *s* - a set, which is an element of *S*.
* *q* - a query set. A set that is used as a parameter for set-containment operation.
* *X* - a set of sets that represents the result of the query.
* *x* - a set, which is an element of *X*.

## Set existence

Given a query set *q*, that consists of elements *q_1* ... *q_n*, we compare the sizes of posting lists that correspond to the keys *q_1* ... *q_n*. Then we retrieve the smallest posting list and check if *q* belongs to this posting list.

## Subset existence

Given a query set *q*, that consists of elements *q_1* ... *q_n*, we retrieve all the posting lists that exist in the index and correspond to the keys *q_1* ... *q_n*. Some keys can be ommited, since we search for subsets. Once a posting list is retrieved its elements (sets) are hashed in the following way. We map a set represetative that is stored in the posting (in our case it is the whole set) to the number of occurrences among all the retrieved posting lists. Lets call this mapping *H*. For example, if a set *y* appears in the posting lists that correspond to the keys *q_1*, *q_3* and *q_5*, we will have a mapping *H(y) = 3*. When the hasing is done we can go through the keys in this mapping and pick sets *x* that will belong to the result set *X* as follows. A set *y_i* belongs to *X* if and only if the cardinality of *y_i* is equal to the mapped value for *y_i*, i.e. *H(y\_i) = | y_i |*. More formal *X* = *{ y_i | H(y\_i) }*.

## Superset existence

Given a query set *q*, that consists of elements *q_1* ... *q_n*, we retrieve all the posting lists in the index that correspond to the keys *q_1* ... *q_n*. If some key does not exist in the index we immediately return an empty result set. Otherwise, we make an intersection among all the retrieved posting lists. The result of the intersection is our set of sepersets of *q*, i.e. *X*.

## Retrieval of subsets and supersets

The algorithms for retrieval are exactly the same as for the existence queries. In both subset and superset "existence" algorithms we come to the point where we have the whole set *X*. In case of existence queries we will simply retrieve the information about the cardinality of *X*. Otherwise, in the case of retrieval query, the whole set *X* must be retrieved.