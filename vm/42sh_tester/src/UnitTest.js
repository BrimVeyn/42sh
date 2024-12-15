import React, {useEffect, useState} from 'react';

function useFetch(path) {
  const [data, setData] = useState(null);
  const [error, setError] = useState(null);

  useEffect(() => {
    fetch('/' + path)
      .then((response) => {
        if (!response.ok) {
          throw new Error(`Fetch failed on ${path}`);
        }
        return response.text();
      })
      .then((json) => setData(json))
      .catch((e) => setError(e));
  }, [path]);

  return { data, error };
}

function UnitTest({unit}) {

	const { data, error } = useFetch(unit.input);

	if (error) {
		return <p>Error: {error.message} </p>;
	}

	return (
		<p> {data ? (
			data
		) : (
			"loading..."
		)} </p>
	);
}

export default UnitTest;
