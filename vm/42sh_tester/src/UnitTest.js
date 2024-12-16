import React, {useEffect, useState} from 'react';
import "./UnitTest.css"

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
		<>
			{data ? (
				<p> Input: {data} </p>
			) : (
				<p> loading... </p>
			)}

			<div className="buttonsContainer">
				<button className="outputButton">
					Ouput (stdout)
				</button>
				<button className="errorButton">
					Error (stderr)
				</button>
				<button className="exitCodeButton">
					Exit code ($?)
				</button>
			</div>
		</>
	);
}

export default UnitTest;
